#ifndef DATA_H
#define DATA_H

struct User{
    int msg_id;
    char nick[20];
};
struct Channel{
    int id;
    char name[20];
    int num_users;
    FILE *fPtr;
    struct User *users;
};

struct Message{
    long type;
    int pid;
    char nick[20];
    char chat[50];
    char content[512];
    int task;    
};
#endif
