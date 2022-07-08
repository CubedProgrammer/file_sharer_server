#ifndef Included_msg_h
#define Included_msg_h
#define UPLOADER 13
#define RECEIPIENT 17
#define ROOMNUM 19
#define JOINSUCC 23
#define JOINFAIL 29
#define QUIT 31
#define SENDFILE 37
#define CLOSEROOM 41

typedef char unsigned fs_msg_t;
void get_msg_name(fs_msg_t msg, char *buf);

#endif
