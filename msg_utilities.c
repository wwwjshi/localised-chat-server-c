#include "msg_utilities.h"
#include <string.h>

/*
Functions for message
    - parsing message from client
    - make reply messages to client
*/

// return type of message
short get_type(char message[]) {
    void * ptr;
    ptr = message;
    short * type_ptr;
    type_ptr = ptr;
    return *type_ptr;
}

// make RECV or RECVVONT msg base on type
void get_re_msg(char msg[], char re_msg[], char identifier[], short request_type) {
    short re_type = (short) RECV;
    if(request_type == SAYCONT) {
        re_type = (short) RECVCONT;
    }
    memcpy(re_msg, &re_type, TYPE_SIZE);
    memcpy(re_msg + TYPE_SIZE, identifier, ID_SIZE);
    memcpy(re_msg + TYPE_SIZE + ID_SIZE, msg + TYPE_SIZE, CONTENT_SIZE - TERMINATE_SIZE);
    memcpy(re_msg + TYPE_SIZE + ID_SIZE + CONTENT_SIZE, msg + MESSAGE_SIZE - TERMINATE_SIZE, TERMINATE_SIZE);
}

// get messaeg type, identifer, domain
int parse_gevent_msg(char * message, short * type, char * identifier, char * domain) {
    // only accept conncetion message and handler message
    *type = *(short *) type;
    memcpy(identifier, message + TYPE_SIZE, ID_SIZE);
    memcpy(domain, message + TYPE_SIZE + ID_SIZE, DOMAIN_SIZE);
    return 0;
}


/*
Functions for fifo
    - parsing message from client
    - make reply messages to client
*/