#pragma once
#include<list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm>
#include <string>

using std::list;

typedef enum commands{
    HELP,
    CINIT,
    CR,DE,REQ,REL,TO,
    LIST,
    PR,
    PARSE_ERR
}commands;

typedef enum state{
    READY,
    RUNNING,
    BLOCKED,
}state;

typedef enum priority{
    INIT,
    USER,
    SYSTEM,
}priority;


struct RCB;

struct Resource{
    char* rid;
    int waitNum;
    int occNum;
};

struct PCB{
    char* pid;
    state status;
    priority prior;
    PCB* parent;
    list<PCB*>* children;
    list<Resource*>* resource;
};

struct RCB{
    char* rid;
    int status;
    int max;
    list<char*>* waitList;//等待资源的名单
};

const char* char_status(state s){
    switch(s){
        case RUNNING: return "RUNNING";
        case BLOCKED: return "BLOCKED";
        case READY:   return "READY";
    }
    return "ERR";
}

const char* char_priority(priority p){
    switch(p){
        case INIT:return "INIT";
        case SYSTEM:return "SYSTEM";
        case USER:return "USER";
    }
    return "ERR";
}

Resource* create_resource(const char* name){
    Resource* r=(Resource*)malloc(sizeof(Resource));
    r->rid=(char*)malloc(strlen(name)+1);
    strcpy(r->rid,name);
    r->waitNum=0;
    r->occNum=0;
    return r;
}

RCB* create_rcb(const char* name,int num){
    RCB* r=(RCB*)malloc(sizeof(RCB));
    r->rid=(char*)malloc(strlen(name)+1);
    strcpy(r->rid,name);
    r->max=num;
    r->status=num;
    r->waitList=new list<char*>();
    return r;
}


PCB* create_pcb(const char* name,priority prior){
    PCB* p=(PCB*)malloc(sizeof(PCB));
    p->pid=(char*)malloc(strlen(name)+1);
    strcpy(p->pid,name);
    p->parent=NULL;
    p->prior=prior;
    p->status=READY;
    p->children=new list<PCB*>();
    p->resource=new list<Resource*>();
    return p;
}

bool in_resource(PCB* p,char* rid){
    for(auto i:*(p->resource)){
        if(strcmp(rid,i->rid)==0)
            return true;
    }
    return false;
}

//不检测安全性 默认传入参数均有效
Resource* get_resource(PCB* p,char* rid){
    if(p==NULL||rid==NULL)
        return NULL;
    for(auto i:*(p->resource)){
        if(strcmp(rid,i->rid)==0)
            return i;
    }
    return NULL;
}

void del_pcb(PCB* p){
    p->children->clear();
    p->resource->clear();
}

void del_resource(Resource* r){
    free(r->rid);
}

void del_rcb(RCB* r){
    r->waitList->clear();
}
