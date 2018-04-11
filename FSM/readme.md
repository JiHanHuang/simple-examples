# JiHan   有限状态机的一个Demo
    2018.1.17
标签 ： FSM

---
## 简介
FSM_C是针对循环状态机，并且接口及其简洁化，能够顺利运行和编译。
cfio3_FSM是针对cfio3的一个Demo，对于有分支的状态机比较适用，接口多，清晰可读并且更灵活。

 
----------
## v.0.1
这里我主要记录的是在设计中遇到的两种方案：
 1. 由输入决定状态
```cpp
void SM_C_input(int type,SM_VAR *c_var){
    switch (type){
    case msg_data:
        if(c_SM_init == SM_get_state(c_var)){
            client_SM_init2wait(c_var);
        }else if()
        if(c_SM_wait != SM_get_state(client_SM_var[c_idx])){ //存在多对一的情况
            client_SM_work2wait(client_SM_var[c_idx]);
        }
        break;    
    case msg_data_get_end:
        if(c_SM_work != SM_get_state(client_SM_var[c_idx])){
            client_SM_wait2work(client_SM_var[c_idx]);
        }
        break;    
    case msg_create_file_begin:
        if(c_SM_wait != SM_get_state(client_SM_var[c_idx])){
            client_SM_work2wait(client_SM_var[c_idx]);
        }
        break;    
    case msg_create_file_end:
        if(c_SM_work != SM_get_state(client_SM_var[c_idx])){
            client_SM_wait2work(client_SM_var[c_idx]);
        }
        break;    
    case msg_data_write_begin:
        DEBUG("master>%d<: once work over!\n",rank);
        break;    
    case msg_data_write_end:
        break;    
    case msg_exit:
        if(c_SM_finish != SM_get_state(client_SM_var[c_idx])){
            client_SM_work2finish(client_SM_var[c_idx]);
        }
        break;
    default:
        DEBUG("Error:invalid msg type!\n");
        break;
    }
}
```
 2. 由状态处理输入
```cpp
void SM_C_input(int type,SM_VAR *c_var){
    switch (SM_get_state(c_var))
	{
	case c_SM_init:
		client_SM_init2wait(c_var);  //这里用switch的话，就是两层。
		if(msg_exit == type){
			client_SM_wait2work(c_var);
			client_SM_work2finish(c_var);
		}
	break;    
	case c_SM_wait:
		if((type == msg_create_file_end)||(type == msg_data_get_end)||(type == msg_data_write_end)){
			client_SM_wait2work(c_var);
		}else{
			client_SM_anyone2error(c_var);
		}
	break;    
	case c_SM_work:
		if((type == msg_data)||(type == msg_create_file_begin)||(type == msg_data_write_begin)){
			client_SM_work2wait(c_var);
		}else if(type == msg_exit){
			client_SM_work2finish(c_var);
		}else{
			client_SM_anyone2error(c_var);
		}
	break;    
	case c_SM_finish:
		DEBUG("client SM state is finish!\n");
	break;
	case c_SM_error:
		DEBUG("client SM state is error!\n");
	break;    
	default:
		DEBUG("Error:invalid SM state!\n");
	break;
	}
}
``` 
上述两种，一种重心在输入，一种在自己的状态，很明显，第一种的思路使得状态机变不成状态机，输入的数据为中心过后， 会使得状态由输入的变换随意 更改，导致非常混乱。而第二种，由状态处理输入，就使得状态更加明确，自身状态结构更加明了，说白了，状态时老大，输入的数据看我心情处理。这样才能是状态机。也就会使得状态机不鸡肋。