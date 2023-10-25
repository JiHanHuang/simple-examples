#include <stdio.h>
#include <global.h>
#include <cfio3_log.h>
#include <cfio3_FSM.h>
#include <string.h>

/*********************************************************
--JiHan--
针对不同进程类型的状态机
主要用于标识每个进程的状态，并进行对应的进程转换。

注：
	现在的设计还不合理，特别是应用的时候。

	第二版进行更改，由原来的消息决定状态更改为由状态来判断
	消息。具体区别见笔记。
	
*********************************************************/


/**************************SM**********************************/
SM_VAR* SM_init(int rank,int type,char *message){
	SM_VAR *var = new SM_VAR;
	var->rank = rank;
	var->procs_type = type;
	strncpy(var->message,message,256);
	if(proc_client == type){
		var->state = c_SM_init;
	}else if(proc_master == type){
		var->state = m_SM_init;
	}else if(proc_server == type){
		var->state = s_SM_init;
	}else{
		return NULL;
	}
	return var;
}

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
			DEBUG("Error:invalid client SM state:%d!\n",SM_get_state(c_var));
		break;
		}
	
}

/*
void SM_M_next(SM_VAR *c_var){
	SM_M_input(-1,c_var);
}
*/

void SM_M_input(int type,SM_VAR *c_var){
	switch (SM_get_state(c_var))
		{
		case m_SM_init: 
			master_SM_init2wait(c_var);  //这里用switch的话，就是两层。
			if(msg_exit == type){
				master_SM_wait2work(c_var);
				master_SM_work2finish(c_var);
			}
		break;	
		case m_SM_wait: 			
			master_SM_wait2work(c_var);
		break;	
		case m_SM_work: 
			if(type == msg_exit){
				master_SM_work2finish(c_var);
			}else{
				master_SM_work2wait(c_var);
			}
		break;	
		case m_SM_finish: 
			DEBUG("client SM state is finish!\n");
		break;
		case m_SM_error: 
			DEBUG("client SM state is error!\n");
		break;	
		default:
			DEBUG("Error:invalid master SM state:%d!\n",SM_get_state(c_var));
		break;
		}
	
}

void SM_S_input(int type,SM_VAR *c_var){
	switch (SM_get_state(c_var))
		{
		case s_SM_init: 
			server_SM_init2wait(c_var);  //这里用switch的话，就是两层。
			if(msg_exit == type){
				server_SM_wait2finish(c_var);
			}
		break;	
		case s_SM_wait: 
			if(msg_exit == type){
				server_SM_wait2finish(c_var);
			}else if((type == msg_data)||(type == msg_create_file_begin)||(type == msg_data_write_begin)){
				server_SM_wait2work(c_var);
			}else{
				server_SM_anyone2error(c_var);
			}
		break;	
		case s_SM_work: 
			if((type == msg_create_file_end)||(type == msg_data_get_end)||(type == msg_data_write_end)){
				server_SM_work2wait(c_var);
			}else{
				server_SM_anyone2error(c_var);
			}
		break;	
		case s_SM_finish: 
			DEBUG("server SM state is finish!\n");
		break;
		case s_SM_error: 
			DEBUG("server SM state is error!\n");
		break;	
		default:
			DEBUG("Error:invalid server SM state:%d!\n",SM_get_state(c_var));
		break;
		}
	
}


void SM_finilize(SM_VAR* var){
	delete var;
}

SM_State SM_get_state(SM_VAR* var){
	if(var->rank >= 0){
		return var->state;
	}else{
		DEBUG("%s rank is -1\n",var->message);
		return -1;
	}
}

/**************************client**********************************/
void client_SM_init2wait(SM_VAR* var){
	if(c_SM_init == var->state){	
		var->state = c_SM_wait;
	}else{
		strncpy(var->message,"init -> wait",256);
		client_SM_anyone2error(var);
	}
}

void client_SM_wait2work(SM_VAR* var){
	if(c_SM_wait == var->state){
		var->state = c_SM_work;
	}else{
		strncpy(var->message,"wait -> work",256);
		client_SM_anyone2error(var);
	}
}

void client_SM_work2wait(SM_VAR* var){
	if(c_SM_work == var->state){
		var->state = c_SM_wait;
	}else{
		strncpy(var->message,"work -> wait",256);
		client_SM_anyone2error(var);
	}
}

void client_SM_work2finish(SM_VAR* var){
	if(c_SM_work == var->state){
		var->state = c_SM_finish;
	}else{
		strncpy(var->message,"work -> finish",256);
		client_SM_anyone2error(var);
	}
}

void client_SM_anyone2error(SM_VAR* var){
	DEBUG("client SM error! CS:%d msg:%s\n",var->state,var->message);
	var->state = c_SM_error;
}

void client_SM_error2init(SM_VAR* var){
	if(c_SM_error == var->state){
		var->state = c_SM_init;
		strncpy(var->message,"client",256);
	}else{
		strncpy(var->message,"error -> init",256);
		client_SM_anyone2error(var);
	}
}


/****************************master**********************************/
void master_SM_init2wait(SM_VAR* var){
	if(m_SM_init == var->state){
		var->state = m_SM_wait;
	}else{
		strncpy(var->message,"init -> wait",256);
		master_SM_anyone2error(var);
	}
}

void master_SM_wait2work(SM_VAR* var){
	if(m_SM_wait == var->state){
		var->state = m_SM_work;
	}else{
		strncpy(var->message,"wait -> work",256);
		master_SM_anyone2error(var);
	}
}

void master_SM_work2wait(SM_VAR* var){
	if(m_SM_work == var->state){
		var->state = m_SM_wait;
	}else{
		strncpy(var->message,"work -> wait",256);
		master_SM_anyone2error(var);
	}
}

void master_SM_work2finish(SM_VAR* var){
	if(m_SM_work == var->state){
		var->state = m_SM_finish;
	}else{
		strncpy(var->message,"work -> finish",256);
		master_SM_anyone2error(var);
	}
}

void master_SM_anyone2error(SM_VAR* var){
	DEBUG("master SM error! CS:%d msg:%s\n",var->state,var->message);
	var->state = m_SM_error;
}

void master_SM_error2init(SM_VAR* var){
	if(m_SM_error == var->state){
		var->state = m_SM_init;
		strncpy(var->message,"master",256);
	}else{
		strncpy(var->message,"error -> init",256);
		master_SM_anyone2error(var);
	}
}

/****************************server**********************************/
void server_SM_init2wait(SM_VAR* var){
	if(s_SM_init == var->state){
		strncpy(var->message,"init -> wait",256);
		var->state = s_SM_wait;
	}else{
		server_SM_anyone2error(var);
	}
}

void server_SM_wait2work(SM_VAR* var){
	if(s_SM_wait == var->state){
		var->state = s_SM_work;
	}else{
		strncpy(var->message,"wait -> work",256);
		server_SM_anyone2error(var);
	}
}

void server_SM_work2wait(SM_VAR* var){
	if(s_SM_work == var->state){
		var->state = s_SM_wait;
	}else{
		strncpy(var->message,"work -> wait",256);
		server_SM_anyone2error(var);
	}
}

void server_SM_wait2finish(SM_VAR* var){
	if(s_SM_wait == var->state){
		var->state = s_SM_finish;
	}else{
		strncpy(var->message,"work -> finish",256);
		server_SM_anyone2error(var);
	}
}

/*
void server_SM_wait2handle(SM_VAR* var){
	if(s_SM_wait == var->state){
		var->state = s_SM_handle_msg;
	}else{
		server_SM_anyone2error(var);
	}
}

void server_SM_handle2wait(SM_VAR* var){
	if(s_SM_handle_msg == var->state){
		var->state = s_SM_wait;
	}else{
		server_SM_anyone2error(var);
	}
}

void server_SM_handle2write(SM_VAR* var){
	if(s_SM_handle_msg == var->state){
		var->state = s_SM_write_data;
	}else{
		server_SM_anyone2error(var);
	}
}

void server_SM_write2wait(SM_VAR* var){
	if(s_SM_write_data == var->state){
		var->state = s_SM_wait;
	}else{
		server_SM_anyone2error(var);
	}
}

void server_SM_write2finish(SM_VAR* var){
	if(s_SM_write_data == var->state){
		var->state = s_SM_finish;
	}else{
		server_SM_anyone2error(var);
	}
}*/

void server_SM_anyone2error(SM_VAR* var){
	DEBUG("server SM error! CS:%d msg:%s\n",var->state,var->message);
	var->state = s_SM_error;
}

void server_SM_error2init(SM_VAR* var){
	if(s_SM_error == var->state){
		var->state = s_SM_init;
		strncpy(var->message,"server",256);
	}else{
		strncpy(var->message,"error -> init",256);
		server_SM_anyone2error(var);
	}
}

