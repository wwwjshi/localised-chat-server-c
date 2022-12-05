#ifndef MSG_UTILITIES
#define MSG_UTILITIES

#define MESSAGE_SIZE (2048)
#define TYPE_SIZE (2)
#define ID_SIZE (256)
#define DOMAIN_SIZE (256)
#define CONTENT_SIZE (1789)
#define TERMINATE_SIZE (1)
#define MAX_PIPE_NAME (256 + 256 + 3)

enum msg_type {CONNECT, SAY, SAYCONT, RECV, RECVCONT, PING, PONG, DISCONNECT};

short get_type(char message[]);
void get_re_msg(char msg[], char re_msg[], char identifier[], short request_type);
int parse_gevent_msg(char * message, short * type, char * identifier, char * domain);


#endif