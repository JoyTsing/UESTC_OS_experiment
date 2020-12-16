#include "./prm.hpp"
#include "./pmMange.hpp"

#define BUFFER 2048
#ifdef _WIN32
static char buffer[BUFFER];
char* readline(char* prompt){
    fputs(prompt,stdout);
    fgets(buffer,BUFFER,stdin);
    char* cpy=(char*)malloc(strlen(buffer)+1);
    strcpy(cpy,buffer);
    cpy[strlen(cpy)-1]='\0';
    return cpy;
}
void add_history(char* unused){}
#else
//解决linux下小键盘移动出现[[^C的问题
//以下两个库解决小键盘移动失效的问题
#include<editline/readline.h>
#include<editline/history.h>
#endif

pmMange* pm=new pmMange();
bool hava_init=false;

void errMsg(const char* fmt,...){
    char* err;

    va_list va;
    va_start(va,fmt);

    err=(char*)malloc(512);
    vsnprintf(err,511,fmt,va);
    err=(char*)realloc(err,strlen(err)+1);

    va_end(va);
    printf("%s",err);
}


/**
 * 函数名，参数，条件
 */
#define SASSERT(args,cond,fmt,...)\
    if(!(cond)){ \
        errMsg(fmt,##__VA_ARGS__);\
        return PARSE_ERR;\
}
/**
 * 函数名，收到参数，期望参数
 */
#define SASSERT_NUM(com, args, num) \
    SASSERT(args, args == num,\
            "command '%s' passed incorrect number of arguments. Got %i, Expected %i.\n", \
            com, args, num)

#define SASSERT_INIT()\
    SASSERT(0,hava_init==true,\
            "init not executed\n")

#define READY_QUIT(input)\
    if(strcmp(input,"exit")==0){\
        return 0;\
}

void com_help();
void builtin_load(char*);

commands handle(char* msg){
    if(msg==NULL||strcmp(msg," ")==0||strcmp(msg,"\n")==0
       ||strcmp(msg,"\t")==0||strcmp(msg,"")==0)
        return PR;
    const char* split=" ";
    char* token=strtok(msg,split),*tmp=token;
    int count=0;
    char* arg[2];

    while(tmp!=NULL){
        tmp=strtok(NULL," ");
        if(count<2)
            arg[count]=tmp;
        count++;
    }
    int parNum=count-1;

    if(strcmp(token,"help")==0){
        SASSERT_NUM("help",parNum,0);
        com_help();
        return HELP;
    }
    else if(strcmp(token,"load")==0){
        SASSERT_NUM("load",parNum,1);
        builtin_load(arg[0]);
        return LOAD;
    }
    else if(strcmp(token,"init")==0){
        SASSERT_NUM("init",parNum,0);
        pm->init();
        hava_init=true;
        printf("%s process is running\n",pm->current->pid);
        return CINIT;
    }
    else if(strcmp(token,"cr")==0){
        SASSERT_NUM("cr",parNum,2);
        SASSERT_INIT();
        switch(atoi(arg[1])){
            case 0:
            case 1:
            case 2:break;
            default:return PARSE_ERR;
        }
        if(pm->create(arg[0],atoi(arg[1])))
            printf("process %s is running.\n",pm->current->pid);
        return CR;
    }
    else if(strcmp(token,"de")==0){
        SASSERT_NUM("de",parNum,1);
        SASSERT_INIT();
        if(strcmp("init",arg[0])==0){
            printf("init can't be destroy\n");
            return DE;
        }
        pm->delete_parent(arg[0]);
        pm->destroy(arg[0]);
        printf("delete success,now process %s running\n",pm->current->pid);
        return DE;
    }
    else if(strcmp(token,"req")==0){
        SASSERT_NUM("req",parNum,2);
        SASSERT_INIT();
        char* ret=pm->request(arg[0],atoi(arg[1]));
        if(ret==NULL)
            return REQ;
        if(strcmp(ret,pm->current->pid)!=0){
            printf("process %s is runing. ",pm->current->pid);
            printf("process %s is blocked.\n",ret);
        }else{
            printf("process %s requests %d %s\n",ret,atoi(arg[1]),arg[0]);
        }
        return REQ;
    }
    else if(strcmp(token,"rel")==0){
        SASSERT_NUM("rel",parNum,2);
        SASSERT_INIT();
        if(pm->release(pm->current->pid,arg[0],atoi(arg[1])))
            pm->ProcessInfo(pm->current->pid);
        return REL;
    }
    else if(strcmp(token,"to")==0){
        SASSERT_NUM("to",parNum,0);
        SASSERT_INIT();
        char* r=pm->timeout();
        printf("process %s is running. ",pm->current->pid);
        if(strcmp(r,pm->current->pid)!=0)
            printf("process %s is ready.",r);
        printf("\n");
        return TO;
    }
    else if(strcmp(token,"list")==0){
        SASSERT_NUM("list's command",parNum,1);
        SASSERT_INIT();
        if(strcmp(arg[0],"pr")==0){
            pm->listProcess();
        }else if(strcmp(arg[0],"res")==0){
            pm->listResource();
        }else if(strcmp(arg[0],"ready")==0){
            pm->printReady();
        }else if(strcmp(arg[0],"block")==0){
            pm->printfBlock();
        }else
            return PARSE_ERR;
        return LIST;
    }
    else if(strcmp(token,"pr")==0){
        SASSERT_NUM("pr",parNum,1);
        SASSERT_INIT();
        pm->ProcessInfo(arg[0]);
        return PR;
    }
    return  PARSE_ERR;
}


void com_help(){
    printf("Usage\n");
    printf(" load <filename> :load commands from file,such as:test.txt\n");
    printf(" init :boot to start\n");
    printf(" cr <name> <priority> :create process priority only 1 or 2.\n");
    printf(" de <name> :delete process.\n");
    printf(" req <resource name> <# of units> :resource.\n");
    printf(" rel <resource name> <# of units> :release resource.\n");
    printf(" to :timeout\n");
    printf("Look for processes status and resources status：\n");
    printf(" list pr :list all processes and their status.\n");
    printf(" list res :list all resource's state.\n");
    printf(" list ready :list all ready processes\n");
    printf(" list block :list all blocked process\n");
    printf(" pr <name> :print pcb information about a given process.\n\n");
}

void command_print(commands c){
    switch(c){
        case HELP:
        case CINIT:
        case CR:
        case DE:
        case REQ:
        case REL:
        case TO:
        case LOAD:
        case LIST:
        case PR:break;
        default:printf("Parse Error\n");
    }
}

void builtin_load(char* filename){
    char buf[BUFFER];
    FILE* fp;
    if((fp=fopen(filename,"r"))==NULL){
        printf("File %s Not existed\n",filename);
        return;
    }
    while(fgets(buf,BUFFER,fp)!=NULL){
        buf[strlen(buf)-1]='\0';
        command_print(handle(buf));
        memset(buf,0,sizeof(buf));
    }
    return;
}

int main(int argc,char* argv[])
{
    puts("Shell version 0.0.1.0,input 'help' to know more");
    puts("press Ctrl+c or input exit to quit\n");
    switch(argc){
        case 1:
            while(1){
                char* input=readline("sh> ");
                READY_QUIT(input);
                add_history(input);
                command_print(handle(input));
                free(input);
            }
            break;
        case 2:
            printf("Load file: %s\n\n",argv[1]);
            builtin_load(argv[1]);
            break;
        default:
            printf("Usage: %s <filename>",argv[0]);
            break;
    }
    return 0;
}

