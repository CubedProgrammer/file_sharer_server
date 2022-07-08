#ifndef Included_room_h
#define Included_room_h
#include<stddef.h>
#include<stdint.h>

struct share_room
{
    size_t rccapa, rccnt;
    uint64_t num;
    int *receivers;
    int uploader;
};

struct room_htable
{
    struct share_room **buckets;
    size_t bcnt, rcnt;
};

extern struct room_htable fs_all_rooms;
void *process_room(void *arg);
int init_rmtable(void);
int insert_room(uint64_t num, int uploader);
void remove_room(uint64_t num);
int join_room(struct share_room *room, int rec);
struct share_room **get_room(uint64_t num);
int init_room(struct share_room *room, int uploader);
void clear_room(struct share_room *room);

#endif
