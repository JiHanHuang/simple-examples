#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>

//JiHan  2018.02.05 
/*****************************
 * 主要是关于Win_post和Win_start
 * 的一个Demo，继之前的一对多的
 * 通信方式进行相关应用测试
 * （能不能跑通还不知道）。
 ******************************/

void main(int argc,char *argv[]){
    int rank,nprocs;
    int namelen,i,j;
    char procname[MPI_MAX_PROCESSOR_NAME];
    char buf[256];
    int reqest,winFlag;
    int ABsize;
    double *A,*B;
    double s_time,e_time,f_time = 0;
	int all_rank[100];
    MPI_Request req;
    MPI_Status status,winstatus;
    MPI_Win nwin;
	MPI_Group group,c_group,s_group,p_group;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Get_processor_name(procname,&namelen);

	for(i= 0;i<nprocs;i++){
		all_rank[i] = rank;
	}
	
    printf("Hello World! process %d of %d on %s\n",rank,nprocs,procname);
    fflush(stdout);
    ABsize = 1024 *1024 *100 /sizeof(double);
    
    f_time = 0;
    printf("***************************\n");
    A = (double *)malloc(ABsize * sizeof(double));
    MPI_Win_create(A,ABsize*sizeof(double),sizeof(double),MPI_INFO_NULL,MPI_COMM_WORLD,&nwin);

	MPI_Comm_group(MPI_COMM_WORLD,&group);
	MPI_Group_incl(group,1,all_rank,&s_group);
	
	MPI_Group_excl(group,1,all_rank,&c_group);

    int g_size;
    MPI_Group_size(c_group,&g_size);

    printf("group size:%d\n",g_size);
	
    for(i=0;i<4;i++){
    if(0 != rank){
        sprintf(buf,">%d<  hello procs,num:%d!\n",rank,i);
        B = (double *)malloc(ABsize * sizeof(double));
        for(j=0;j<ABsize;j++){
            B[j] = j+i;
        }
        B[9] = rank;
		
        printf("compute over!  time:%d \n",i);

        memcpy(A,B,ABsize*sizeof(double));

        printf(">%d< send begin!  time:%d \n",rank,i);
        fflush(stdout);
        MPI_Send(buf,sizeof(buf),MPI_BYTE,0,0,MPI_COMM_WORLD);
        printf("send over!  time:%d \n",i);
        fflush(stdout);
        //目标进程涉及的组，这里是向server进程开放窗口，就是s_group
//		MPI_Win_post(s_group,MPI_MODE_NOCHECK,nwin);
		MPI_Win_post(s_group,MPI_MODE_NOPUT,nwin);
        s_time = MPI_Wtime();
        sleep(1);
		MPI_Win_wait(nwin);

        e_time = MPI_Wtime();
        printf("\n-ccc----%f-------\n",e_time-s_time);
        free(B);
    }else{
        int k=0;
        double *T[100];
        s_time = MPI_Wtime();
        //源进程涉及的组，这里是向client进程窗口操作，就是c_group。一次性接收c_group中所有的process
        MPI_Win_start(c_group,MPI_MODE_NOCHECK,nwin);
        for(k=0;k<nprocs-1;k++){
        printf(">%d< recv begin!  time:%d \n",rank,i);
        MPI_Recv(buf,sizeof(buf),MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
        printf(">%d<  I get it %d,msg:%s\n",rank,i,buf);

        T[k] = (double *)malloc(ABsize * sizeof(double));
        MPI_Get(T[k],ABsize*sizeof(double),MPI_DOUBLE,status.MPI_SOURCE,0,ABsize*sizeof(double),MPI_DOUBLE,nwin);


        }
        MPI_Win_complete(nwin);
        for(k=0;k<nprocs-1;k++){
        for(j = 0;j<10;j++){
            printf("%f, ",T[k][j]);
        }
        printf("\n");
        }
        e_time = MPI_Wtime();
        f_time = e_time-s_time+f_time;
        printf("-sss---%f-------\n",e_time-s_time);

        fflush(stdout);
    }
}

    if(0 == rank){
        printf("all get time:%f\n",f_time);
        fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Group_free(&c_group);
    MPI_Group_free(&s_group);
    free(A);
    MPI_Win_free(&nwin);
    MPI_Finalize();

}
