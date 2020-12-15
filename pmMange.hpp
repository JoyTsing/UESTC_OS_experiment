#pragma once
#include "./prm.hpp"

class pmMange{
public:
    pmMange();
    ~pmMange();
    list<RCB*> rcb;
    list<PCB*> pcb;
    PCB* current;

    void init();
    bool create(char* name,int p);
    void schedule();
    void destroy(char* pid);
    void listProcess();
    void ProcessInfo(char* name);

    char* request(char* rid,int unit);
    bool release(char*pid ,char* rid,int unit);
    void listResource();
    char* timeout();//返回之前的进程pid

    void printReady();
    void printfBlock();
private:
    list<PCB*> readyList[3];//不同优先级的就绪列表
    bool in_rcblist(char* rid);
    bool in_pcblist(char* name);
    PCB* search(char* pid);
    RCB* search_rcb(char* rcb);
    bool is_blocked(char* pid);
};


pmMange::pmMange(){
    PCB* init=create_pcb("init",INIT);

    readyList[0].push_front(init);
    current=init;
    current->status=RUNNING;
}

void pmMange::init(){
    //init
    current=readyList[0].front();
    //先清空
    while(!rcb.empty()){rcb.pop_front();}
    while(!pcb.empty()){pcb.pop_front();}

    for(int i=0;i<3;i++)
        while(!readyList[i].empty())
            readyList[i].pop_front();
    //重新赋值
    readyList[0].push_front(current);
    pcb.push_back(current);
    for(int i=1;i<=4;i++){
        std::string rid="R"+std::to_string(i);
        RCB* r=create_rcb(rid.c_str(),i);
        rcb.push_back(r);
    }
}

bool pmMange::in_pcblist(char* name){
    for(auto i:pcb){
        if(strcmp(i->pid,name)==0){
            return true;
        }
    }
    return false;
}

bool pmMange::in_rcblist(char* rid){
    for(auto i:rcb){
        if(strcmp(i->rid,rid)==0){
            return true;
        }
    }
    return false;
}

bool pmMange::create(char* name,int p){
    if(!in_pcblist(name)){
        PCB* npcb=create_pcb(name,priority(p));
        npcb->parent=search(current->pid);
        pcb.push_back(npcb);
        readyList[p].push_back(npcb);
        current->children->push_back(npcb);
    }else{
        printf("create faild, process %s hava existed.\n",name);
        return false;
    }
    schedule();
    return true;
}

void pmMange::listProcess(){
    printf(">Process List\n");
    for(auto i:pcb){
        printf(" * pid: <%s> | state: <%s> \n",i->pid,char_status(i->status));
    }
}

void pmMange::listResource(){
    printf(">Resource List\n");
    for(auto i:rcb){
        printf(" * rid: <%s> | resource: %d / %d \n",i->rid,i->status,i->max);
    }
}

PCB* pmMange::search(char* pid){
    if(pid==NULL)
        return NULL;
    for(auto i:pcb){
        if(strcmp(pid,i->pid)==0){
            return i;
        }
    }
    return NULL;
}

RCB* pmMange::search_rcb(char* rid){
    if(rid==NULL)
        return NULL;
    for(auto i:rcb){
        if(strcmp(rid,i->rid)==0){
            return i;
        }
    }
    return NULL;
}

void pmMange::ProcessInfo(char* name){
    PCB* t=search(name);
    if(t!=NULL){
        printf(" * pid: <%s> | state: <%s> | priority: <%s> \n",t->pid,
               char_status(t->status),char_priority(t->prior));
        for(auto i:*(t->resource)){
            printf("   - Rid: %s : Occ: %d Need: %d \n",i->rid,i->occNum,i->waitNum);
        }
    }else{
        printf("process does not exist\n");
    }
}

void pmMange::schedule(){
    if(!readyList[2].empty()){
        current=readyList[2].front();
    }else if(!readyList[1].empty()){
        current=readyList[1].front();
    }else{
        current=readyList[0].front();
    }
    current->status=RUNNING;
    return;
}

char* pmMange::request(char* rid,int unit){
    if(!in_rcblist(rid)){
        printf("Resource %s not exist\n",rid);
        return NULL;
    }
    char* npid=current->pid;
    RCB* r=search_rcb(rid);
    if(unit>r->max){
        printf("Too many resource request for %s,max is %d\n",rid,r->max);
        return NULL;
    }
    if(!in_resource(current,rid)){
        Resource* nres=create_resource(rid);
        current->resource->push_back(nres);
    }

    if(r->status>=unit){
        r->status=r->status-unit;
        Resource* res=get_resource(current,rid);
        res->occNum+=unit;
    }else{
        current->status=BLOCKED;
        Resource* res=get_resource(current,rid);
        res->waitNum+=unit;
        r->waitList->push_back(current->pid);
        readyList[current->prior].remove(current);
    }
    schedule();
    return npid;
}

char* pmMange::timeout(){
    int priority=current->prior;
    char* r=current->pid;
    readyList[priority].remove(current);
    current->status=READY;
    readyList[priority].push_back(current);
    schedule();
    return r;
}

bool pmMange::release(char* pid,char* rid,int unit){
    PCB* cur=search(pid);
    Resource* res=get_resource(cur,rid);
    if(cur==NULL||res==NULL){
        return false;
    }

    if(res->occNum<unit){
        printf("process %s only have %d %s,request exceed\n",
               cur->pid,res->occNum,res->rid);
        return false;
    }
    res->occNum=res->occNum-unit;
    RCB* r=search_rcb(rid);
    r->status=r->status+unit;

    PCB* wait=search(r->waitList->front());
    Resource* waitr =get_resource(wait,rid);

    while(wait!=NULL&&waitr->waitNum<=r->status){
        r->status=r->status-waitr->waitNum;
        r->waitList->remove(wait->pid);

        waitr->occNum=waitr->occNum+waitr->waitNum;
        waitr->waitNum=0;

        if(!is_blocked(wait->pid)){
            //原来想的是会存在一个进程申请多个资源被阻塞的情况 但实际用下来发现
            //最多申请一个被阻塞就不能再跳到该进程了，意思就是只会被一种资源阻塞
            //而不是多种资源
            wait->status=READY;
            readyList[wait->prior].push_back(wait);
        }
        wait=search(r->waitList->front());
        waitr=get_resource(wait,rid);
    }
    schedule();
    return true;
}

bool pmMange::is_blocked(char* pid){
    if(pid==NULL)
        return false;
    for(auto i:rcb){
        list<char*>* tmp=i->waitList;
        for(auto c:*tmp){
            if(strcmp(c,pid)==0){
                return true;
            }
        }
    }
    return false;
}

void pmMange::destroy(char* pid){
    PCB* p=search(pid);
    if(p==NULL){
        printf("process %s not exist\n",pid);
        return;
    }
    p->parent->children->remove(p);
    for(auto res:*(p->resource)){
        release(p->pid,res->rid,res->occNum);
    }

    if(p->status==READY||p->status==RUNNING){
        readyList[p->prior].remove(p);
    }else{
        for(auto res:rcb){
            res->waitList->remove(p->pid);
        }
    }

    for(auto it=p->children->begin();it!=p->children->end();){
        if(strcmp((*it)->parent->pid,p->pid)==0){
            destroy((*it)->pid);
            it=p->children->erase(it);
        }else
            it++;
    }
    del_pcb(p);
    pcb.remove(p);
    schedule();
}

void pmMange::printReady(){
    printf("Ready List\n");
    for(int i=2;i>=0;i--){
        printf("* %s:\n",char_priority(priority(i)));
        printf(" > ");
        for(auto i:readyList[i]){
            printf("%s ",i->pid);
        }
        fputc('\n',stdout);
    }
}

void pmMange::printfBlock(){
    printf("Block List\n");
    for(auto i:rcb){
        printf("* %s:",i->rid);
        for(auto j:*(i->waitList)){
            printf(" %s",j);
        }
        fputc('\n',stdout);
    }
}
