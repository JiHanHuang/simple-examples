#include <stdio.h>

typedef unsigned char State;
typedef State(*Procedure)(void *);//函数指针
enum states{ //状态定义
    s_init,
    s_count,
    s_done,
    s_default  
}; //枚举类型，自动复制对应0-3

typedef struct _SM_AVR//状态机参数封装
{
    int cnt;
    char string;
}SM_VAR;

State step_init(void *arg){
    SM_VAR *p = (SM_VAR *) arg;
    p->cnt = 0;
    printf("CS:init  cnt=%d  NS:count\n",p->cnt);
    return s_count;//下一个状态
}

State step_count(void *arg){
    SM_VAR *p = (SM_VAR *) arg;
    if(p->cnt < 3){
        p->cnt++;
        printf("CS:count  cnt=%d  NS:count\n",p->cnt);
        return s_count;
    }else{
        printf("CS:count  cnt=%d  NS:done\n",p->cnt);
        return s_done;
    }
}

State step_done(void *arg){
    SM_VAR *p = (SM_VAR *) arg;
    p->cnt = 0;
    printf("CS:done  cnt=%d  NS:init\n",p->cnt);
    return s_init;
}

State step_default(void *arg){
    SM_VAR *p = (SM_VAR *) arg;
    printf("wrong state\n");
    return s_init;
}

Procedure Steps[] = {//指向函数名称
    step_init,
    step_count,
    step_done,
    step_default
};

void BestSM(void *invar){
    static State NS = s_init;
    NS = Steps[NS](invar);
    printf("");
}

int main(void){
    SM_VAR var;
    int i;
    for(i=0;i<8;i++){
        BestSM(&var);
    }
    return 0;
}
