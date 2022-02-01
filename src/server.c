#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "data.h"
#include <time.h>
#include <sys/stat.h>
#include <signal.h>

struct User *users = NULL;
int num_users = 0;
struct Channel *channels = NULL;
int num_channels = 0;
int go = 1;
struct Message MSG;


//--------------------USERS---------------------
int is_unique(char *nick)
{
    for (int i =0; i<num_users;i++)
    {
        if(strcmp(nick, users[i].nick)==0)
            return 0;
    }
    return 1;
}
void add_user(int id, char *nick)
{
    if(is_unique(nick)){
        if(num_users == 0)
            users = (struct User*)malloc(sizeof(struct User));
        else
            users = (struct User*)realloc(users, (num_users+1)*sizeof(struct User));
        users[num_users].msg_id = id;
        strcpy(users[num_users].nick,nick);
        num_users++;
        printf("User %s added to the network\n", nick);
    }
    else{
        strcpy(MSG.content,"Can't add user - name not unique\n");
        MSG.task = 9;
    }
}
void show_active_users_in_server(){
    printf("Showing active Users\n");
    char connected_users[500] = "Users connected to chat ";
    for (int i =0; i<num_users;i++){
        strcat(connected_users, users[i].nick);
        strcat(connected_users, " ");
        printf("%d %s \n", users[i].msg_id, users[i].nick);
    }
    strcpy(MSG.content, connected_users);
}

void leave_channel (char *name, struct User user)
{
    for(int i = 0; i<num_channels; i++)
        if(strcmp(name, channels[i].name)==0)
        {
            channels[i].num_users--;
            struct User *tmp = (struct User*)malloc(channels[i].num_users*sizeof(struct User)); 
            int tmp_index=0;
            for(int j = 0; j<channels[i].num_users+1;j++)
            {
                if (strcmp(channels[i].users[j].nick,user.nick)==0)
                    continue;
                else
                    tmp[tmp_index]=channels[i].users[j];
                tmp_index++;
            }
            free (channels[i].users);
            channels[i].users = tmp;
        }
    printf("%s left channel %s succesfully.\n", user.nick, name);
}

void delete_user(struct User user)
{
    for (int i = 0; i<num_channels; i++)
    {
        for (int j = 0; j<channels[i].num_users; j++)
        {
            if(strcmp(channels[i].users[j].nick,user.nick)==0)
            {
                leave_channel(channels[i].name,user);
                break;                
            }
        }
    }
    for(int i = 0; i<num_users; i++)
        if(strcmp(user.nick, users[i].nick)==0)
        {
            num_users--;
            struct User *tmp = (struct User*)malloc(num_users*sizeof(struct User));
            int tmp_index=0;
            for(int j = 0; j<num_users+1;j++)
            {
                if (strcmp(users[j].nick,user.nick)==0)
                    continue;
                else
                    tmp[tmp_index]=users[j];
                tmp_index++;
            }
            free (users);
            users = tmp;
        }
    printf("%s deleted succesfully.\n", user.nick);

}
int find_user(char* nick)
{
    for(int i = 0; i<num_users; i++)
        if(strcmp(nick, users[i].nick)==0)
            return i;
}
//----------------------------------------------

//------------------CHANNELS--------------------
void create_file(char *name){
    
    char FILE_NAME[522];
    strcpy(FILE_NAME, name);
    strcat(FILE_NAME,".txt");
    for(int i = 0; i<num_channels; i++)
    {
        if(strcmp(name, channels[i].name)==0)
            channels[i].fPtr = fopen(FILE_NAME, "w");
    }
    printf("CREATED A TEXT FILE FOR MESSAGE HISTORY\n");
}

void add_channel(char *name)
{
    if(num_channels == 0)
        channels = (struct Channel*)malloc(sizeof(struct Channel));
    else
        channels = (struct Channel*)realloc(channels, (num_channels+1)*sizeof(struct Channel));
        channels[num_channels].id = num_channels;
        strcpy(channels[num_channels].name,name);
        channels[num_channels].num_users = 0;
        num_channels++;
    printf("added channel %s\n", name);
    create_file(name);
}

void show_active_users_in_channel(char *name)
{
    char connected_users[500] = "Users active in channel ";
    strcat(connected_users, name);
    for(int i = 0; i<num_channels; i++)
    {
        if(strcmp(name, channels[i].name)==0)
        {
            printf("Showing active Users in channel named %s \n", name);
            for (int j = 0; j<channels[i].num_users; j++){
                printf("%s ", channels[i].users[j].nick);
                strcat(connected_users, " "); 
                strcat(connected_users, channels[i].users[j].nick);
            }
            printf("\n");
        }
    }
    strcpy(MSG.content, connected_users);
}

void show_active_channels()
{
    printf("Showing active channels: \n"); 
    char active_channels[500] = "Channels active in a server ";
    for(int i = 0; i < num_channels; i++){
            printf("Channel ID: %d, Channel Name: %s, Number of users in channel:  %d\n",channels[i].id, channels[i].name, channels[i].num_users);
            strcat(active_channels, channels[i].name);
            strcat(active_channels, " "); 
    }
    strcpy(MSG.content, active_channels);
}

int join_channel(char *name, struct User user)
{
    for(int i = 0; i<num_channels; i++)
        if(strcmp(name, channels[i].name)==0)
        {
            if(channels[i].num_users == 0)
            {
                channels[i].users = (struct User*)malloc(sizeof(struct User));
                channels[i].users[channels[i].num_users] = user;
                channels[i].num_users++;
            }
            else
            {
                channels[i].num_users++;
                channels[i].users = (struct User*)realloc(channels[i].users, channels[i].num_users*sizeof(struct User));
                channels[i].users[channels[i].num_users-1] = user;
            }
            printf("%s joined channel %s\n", user.nick, channels[i].name);
            
            return 1;
        }
    add_channel(name);
    join_channel(name, user);

    return 1;
}

void send_msg(char *name, int sid, char* nick)
{
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char tmp[522]="";
    char time_formated [522];
    sprintf(time_formated, "%02d:%02d ", timeinfo->tm_hour, timeinfo->tm_min);
    strcat(tmp,time_formated);
    strcat(tmp, MSG.content);
    strcpy(MSG.content, tmp);
    char FILE_NAME[522];
    strcpy(FILE_NAME, name);
    strcat(FILE_NAME,".txt");
    for(int i = 0; i<num_channels; i++)
    {
        if(strcmp(name, channels[i].name)==0)
        {
            FILE *fptr = fopen(FILE_NAME,"a");
            fprintf(fptr, "%s\n", MSG.content);
            fclose(fptr);
            for (int j = 0; j<channels[i].num_users; j++)
            {
                if(strcmp(channels[i].users[j].nick,nick)!=0)
                {
                    MSG.type = channels[i].users[j].msg_id;
                    msgsnd(sid,&MSG,sizeof(struct Message),0);
                }
            }
        }
    }
}
void send_dm_msg(int sid, char* nick, char* target_user)
{
    int index = find_user(target_user);
    MSG.type = users[index].msg_id;
    char tmp[522]="";
    strcat(tmp,"[DM] ");
    strcat(tmp, MSG.content);
    strcpy(MSG.content, tmp);
    msgsnd(sid,&MSG,sizeof(struct Message),0);        
}
//----------------------------------------------
int main(int argc, char* argv[]){
    int sid = msgget(strtol(argv[1],NULL, 10),0600|IPC_CREAT);
    char tmp[550];
    while(go)
    {
        msgrcv(sid,&MSG,sizeof(struct Message),1,0);
        MSG.type=MSG.pid;
        switch (MSG.task)
        {
            case 0:
                add_user(MSG.pid,MSG.nick);
                break;
            case 1:
                show_active_users_in_server();
                break;
            case 2:
                join_channel(MSG.chat, users[find_user(MSG.nick)]);
                break;
            case 3:
                show_active_channels();
                break;
            case 4:
                show_active_users_in_channel(MSG.chat);
                break;
            case 5:
                leave_channel(MSG.chat, users[find_user(MSG.nick)]);
                break;
            case 6:
                MSG.content[strlen(MSG.content)-1]='\0';
                strcpy(tmp,MSG.nick);
                strcat(tmp,": ");
                strcat(tmp, MSG.content);
                strcpy(MSG.content, tmp);
                printf("%s\n",MSG.content);
                send_msg(MSG.chat, sid, MSG.nick);
                break;
            case 7:
                MSG.content[strlen(MSG.content)-1]='\0';
                strcpy(tmp,MSG.nick);
                strcat(tmp,": ");
                strcat(tmp, MSG.content);
                strcpy(MSG.content, tmp);
                printf("%s\n",MSG.content);
                send_dm_msg(sid, MSG.nick, MSG.chat);
                break;
            case 10:
                delete_user(users[find_user(MSG.nick)]);
                break;
                
        }
        msgsnd(sid,&MSG,sizeof(struct Message),0);
    }
    return 0;
}
