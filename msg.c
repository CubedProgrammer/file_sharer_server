#include<string.h>
#include"msg.h"

void get_msg_name(fs_msg_t msg, char *buf)
{
    const char *name;
    switch(msg)
    {
        case UPLOADER:
            name = "UPLOADER";
            break;
        case RECEIPIENT:
            name = "RECEIPIENT";
            break;
        case ROOMNUM:
            name = "ROOMNUM";
            break;
        case JOINSUCC:
            name = "JOINSUCC";
            break;
        case JOINFAIL:
            name = "JOINFAIL";
            break;
        case QUIT:
            name = "QUIT";
            break;
        case SENDFILE:
            name = "SENDFILE";
            break;
        case CLOSEROOM:
            name = "CLOSEROOM";
            break;
        default:
            name = "UNKNOWN";
    }
    strcpy(buf, name);
}
