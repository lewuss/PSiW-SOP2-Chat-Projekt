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

struct Message MSG_OUT;
struct Message MSG_IN;

void show_menu()
{
    printf("1. Show All Users connected to the server.\n");
    printf("2. Join channel.\n");
    printf("3. Show all active channels\n");
    printf("4. Show all users connected to a given channel\n");
    printf("5. Leave Channel\n");
    printf("6. Enter Chatting Mode.\n");
    printf("7. Send a direct Message.\n");
    printf("10. Logout\n");
}

int main(int argc, char* argv[])
{
    int sid = msgget(strtol(argv[2],NULL, 10),0600|IPC_CREAT);
    MSG_OUT.pid = getpid();
    char tmp[500]=" ";
    printf("%d\n",MSG_OUT.pid);
    strcpy(MSG_OUT.nick,argv[1]);
    MSG_OUT.task = 0;
    MSG_OUT.type = 1;
    msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);
    msgrcv(sid,&MSG_IN,sizeof(struct Message),MSG_OUT.pid,0);
    printf("%s", MSG_IN.content);
    if(MSG_IN.task==9)
        return 0;
    while(1)
    {
        show_menu();
        scanf("%d",&MSG_OUT.task);
        switch (MSG_OUT.task)
        {
            case 1:
                msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);
                msgrcv(sid,&MSG_IN,sizeof(struct Message),MSG_OUT.pid,0);
                system("clear");
                printf("%s\n", MSG_IN.content);
                break;
            case 2:
                printf("Enter Channel's name.\n");
                scanf("%s", MSG_OUT.chat);
                msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);
                msgrcv(sid,&MSG_IN,sizeof(struct Message),MSG_OUT.pid,0);
                system("clear");
                break;
            case 3:
                msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);
                msgrcv(sid,&MSG_IN,sizeof(struct Message),MSG_OUT.pid,0);
                system("clear");
                printf("%s\n", MSG_IN.content);
                break;
            case 4:
                printf("Enter Channel's name.\n");
                scanf("%s", MSG_OUT.chat);
                msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);
                msgrcv(sid,&MSG_IN,sizeof(struct Message),MSG_OUT.pid,0);
                system("clear");
                printf("%s\n", MSG_IN.content);
                break;
            case 5:
                printf("Enter Channel's name.\n");
                scanf("%s", MSG_OUT.chat);
                msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);
                msgrcv(sid,&MSG_IN,sizeof(struct Message),MSG_OUT.pid,0);
                system("clear");
                break;
            case 6:
                system("clear");
                printf("Enter Channel's name.\n");
                scanf("%s", MSG_OUT.chat);
                char FILE_NAME[522];
                strcpy(FILE_NAME, MSG_OUT.chat);
                strcat(FILE_NAME,".txt");
                FILE *fptr = fopen(FILE_NAME,"r");
                while(fgets(tmp, sizeof(tmp),fptr) != NULL)
                    printf("%s", tmp);
                fclose(fptr);
                strcpy(MSG_OUT.content,"entry string");
                MSG_OUT.content[strlen(MSG_OUT.content)-1]='\0';
                fgets(MSG_OUT.content, 512, stdin);
                int child_process_id = fork();
                if(child_process_id==0)
                {
                    while(1){
                        msgrcv(sid,&MSG_IN,sizeof(struct Message),MSG_OUT.pid,0);
                        if(strcmp(tmp, MSG_IN.content)!=0)
                            printf("%s \n", MSG_IN.content);
                        strcpy(tmp,MSG_IN.content);
                    }
                }
                else
                {
                    while(1)
                    {   
                        fflush(stdin);
                        fgets(MSG_OUT.content, 512, stdin);
                        if(strcmp(MSG_OUT.content,"q\n")==0)
                            break;
                        msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);
                    }
                    kill(child_process_id, SIGKILL);
                }
                system("clear");
                break;
            case 7:
                system("clear");
                printf("Enter Users's name.\n");
                scanf("%s", MSG_OUT.chat);
                fgets(MSG_OUT.content, 512, stdin);
                fflush(stdin);
                fgets(MSG_OUT.content, 512, stdin);
                msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);                
                break;
            case 10:
                msgsnd(sid,&MSG_OUT, sizeof(struct Message),0);
                msgrcv(sid,&MSG_IN,sizeof(struct Message),MSG_OUT.pid,0);
                return 0;
                break;
        }
        printf("\n");
        
    }

}
