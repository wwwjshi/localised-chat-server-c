#include "server.h"
#include "msg_utilities.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>

enum pipe_op {READ, WRITE, NON};

void get_fifo_name(char fifo_name[], char identifier[], enum pipe_op op) {
    strcat(fifo_name, identifier);
    if(op == READ){
        strcat(fifo_name, "_RD");
    } else if(op == WRITE) {
        strcat(fifo_name, "_WR");
    }
}

// determine if a file is a client _RD fifo for sending 
int check_send_rd(char filename[], char my_rd[]) {
    if(strcmp(filename, my_rd) == 0){
        return -1;
    }
    int len = strlen(filename);
    if(len > 3 && filename[len - 3] == '_' && filename[len - 2] == 'R' &&  filename[len - 1] == 'D') {
        return 0;
    } 
    return -1;
}

void get_fifo_path(char * path_str, char * identifier, char * domain, enum pipe_op op) {
    strcat(path_str, domain);
    if(domain[strlen(domain) - 1] != '/') {
        strcat(path_str, "/");
    }
    get_fifo_name(path_str, identifier, op);
}

// check if a fifo already exist and try create it if not exist
void make_fifo(char * fifo_name) {
    if(mkfifo(fifo_name, 0777) == -1) {
        if(errno != EEXIST) {
            perror("mkfifo()");
            exit(-1);
        }
    }
}

// read/write a fifo
int fifo_operate(char * fifo_name, char * message, enum pipe_op op) {
    int fd;
    if(op == READ){
        fd = open(fifo_name, O_RDONLY);
    } else {
        fd = open(fifo_name, O_WRONLY);
    }
    if(fd == -1) {
        fputs("Error occurred in open fifo", stderr);
        return fd;
    }

    int op_suc;
    if(op == READ){
        op_suc = read(fd, message, MESSAGE_SIZE);
    } else {
        op_suc = write(fd, message, MESSAGE_SIZE);
    }
    if(op_suc == -1) {
        fputs("Error occured in fifo operation", stderr);
        return op_suc;
    }
    close(fd);
    return fd;
}

// check and make domain directory if not exist
void check_domain(char domain[]) {
    if(mkdir(domain, 0777) == -1) {
        if(errno != EEXIST) {
            perror("mkdir() failed");   
            return;
        }
    }
}

int main(int argc, char ** argv) {
    // open gevent for reading
    char * global_pipe = "gevent";
    int gevent_fd;
    make_fifo(global_pipe);
    char message[MESSAGE_SIZE] = {0}; 

    // Store client info for handler
    short type; 
    char identifier[ID_SIZE];
    char domain[DOMAIN_SIZE];
    char rd_fifo[MAX_PIPE_NAME] = {0};
    char wr_fifo[MAX_PIPE_NAME] = {0};
    int id; 
    signal(SIGCHLD, SIG_IGN); // dont care about handler

    while(1){
        rd_fifo[0] = '\0';
        wr_fifo[0] = '\0';

        // read message
        gevent_fd = fifo_operate(global_pipe, &message[0], READ);
        if(gevent_fd < 0) {
            continue;
        }
        // parse message 
        parse_gevent_msg(&message[0], &type, &identifier[0], &domain[0]);

        // operate base on message type
        if(type == CONNECT){ // connection type
            // create fifo
            get_fifo_path(rd_fifo, identifier, domain, READ);
            get_fifo_path(wr_fifo, identifier, domain, WRITE);
            check_domain(domain);
            make_fifo(rd_fifo);
            make_fifo(wr_fifo);

            // spawn handler
            id = fork();
            if(id < 0) { // fail to fork
                perror("fork() failed"); 
                continue;
            } else if(id == 0) { // handler process
                break; // break out of loop and operate as handler
            } else { // global process
                continue; // continue to read
            }

        } else { // other type or invalid msg
            continue; // msg disregarded
        }
    }

    // client handler
    if(id == 0) {
        #ifdef TEST
        printf("%s JOINED %s\n", identifier, domain);  // FOR TESTING
        #endif

        char my_rd[ID_SIZE] = {0};
        get_fifo_name(my_rd, identifier, READ);

        while(1) {
            // open fd and read msg from client
            int request_fd = fifo_operate(wr_fifo, &message[0], READ);
            short request_type = get_type(message);

            // say or saycont msg -> pass to other client in domain
            if(request_type == SAY || request_type == SAYCONT) {

                #ifdef TEST
                int t = (int) request_type;
                printf("id: %s dom: %s tyoe: %d contnent: %s\n", identifier, domain, t, &message[2]);  // FOR TESTING
                #endif


                // make rec_msg
                char re_msg[MESSAGE_SIZE] = {0};
                get_re_msg(message, re_msg, identifier, request_type);

                // send to other client in domain
                DIR * d = opendir(domain); // open domain/directory
                struct dirent * dir; // ptr to a directory elements
                    while((dir = readdir(d)) != NULL) { // read elements in directory
                        char filename[ID_SIZE] = {0};
                        sprintf(filename, dir->d_name, "%s");
                        if(check_send_rd(filename, my_rd) == -1){
                            continue;
                        }
                        char other_rd_path[DOMAIN_SIZE + ID_SIZE] = {0};
                        get_fifo_path(other_rd_path, filename, domain, NON);
                        int wr_suc = fifo_operate(other_rd_path, &re_msg[0], WRITE);
                        if(wr_suc == -1){
                            continue;
                        }
                    }
                closedir(d);
            }
            else if(request_type == DISCONNECT) {
                // remove pipes
                close(request_fd);
                remove(wr_fifo);
                remove(rd_fifo);

                #ifdef TEST
                printf("%s LEAVED %s\n", identifier, domain);  // FOR TESTING
                #endif

                return 0;
            }
            close(request_fd);
        }
        return 0;

    } else {
        close(gevent_fd);
        fprintf(stderr, "server down");
    }

    return 0;

}