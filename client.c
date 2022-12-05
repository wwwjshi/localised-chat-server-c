#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

/* To run
      ./client id domain
e.g.  ./client Bob room1
*/

/* To send message
       type msg term  (separated by space)
 e.g.  1 hello 0   (type=SAY, content=hello, termination=0)
       7 ...   .   (type=DISCONNECTION)
*/
int main(int argc, char * argv[]) {
    char type[2] = {0b0, 0b0};
    char ident[256] = {0};
    char dom[256] = {0};
    strcat(ident, argv[1]);
    strcat(dom, "./");
    strcat(dom, argv[2]);
    strcat(dom, "/");

    char full_path[512] = {0};
    char rd_fifo[512] = {0};
    char wr_fifo[512] = {0};
    strcat(full_path, dom);
    strcat(full_path, ident);
    strcat(rd_fifo, full_path);
    strcat(wr_fifo, full_path);
    strcat(rd_fifo, "_RD");
    strcat(wr_fifo, "_WR");

    char mess[2048];
    memcpy(&mess[0], type, 2);
    memcpy(&mess[2], ident, 256);
    memcpy(&mess[258], dom, 256);
    int fd = open("gevent", O_WRONLY);
    if(fd == -1){
        return 1;
    }
    if(write(fd, mess, sizeof(mess)) == -1){
        return 2;
    }
    close(fd);

    int id = fork();
    if(id == 0) { // child: read
        char message[2048];
        while(1) {
            int re_fd = open(rd_fifo, O_RDONLY);
            if(read(re_fd, &message, 2048) > 0) {
                printf("%s: %s\n", &message[2], &message[258]);
            }
            close(re_fd);
        }
    }


    if(id > 0) { // parent: write
        short msg_type;
        char msg[1789] = {0};
        uint8_t term;
        int t = {0};
        int tm = {0};
        char line[2048];

        while(1) {
            msg_type = -1;
            //printf("Me(%s): ", ident);
            fgets(line, sizeof(line), stdin);
            if(sscanf(line, "%d %s %d", &t, msg, &tm) != 3){
                continue;
            }
            msg_type = (short) t;
            term = (uint8_t) tm;
            //printf("msg type: %d %d\n", msg_type, term);
            char mess1[2048] = {0};
            memcpy(&mess1, &msg_type, 2);
            memcpy(&mess1[2], msg, strlen(msg));
            memcpy(&mess1[2047], &term, 1);
            int wr_fd = open(wr_fifo, O_WRONLY);
            if(write(wr_fd, mess1, sizeof(mess)) == -1){
                puts("Fail to write");
                return 2;
            }
            // } else if(msg_type == 1 || msg_type == 2) {
            //     printf("Me(%s): %s\n", ident, msg);
            // }
            close(wr_fd);

            // kill child and end process when quit
            if(msg_type == 7) {
                int k = kill(id, SIGKILL);
                return 0;
            }
        }
    }
    return 0;
}