#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>

/*********************************************
 *JiHan 2018.02.07
 *MPI lock/unlock and One-Sided communications
 *********************************************/

void main(int argc,char *argv[]){
    int rank,nprocs;
    int namelen,i,j;
    char procname[MPI_MAX_PROCESSOR_NAME];
    char buf[256];
    int reqest,winFlag;
    int ABsize;
    double *A,*B;
    double s_time,e_time,f_time = 0;
    MPI_Request req;
    MPI_Status status,winstatus;
    MPI_Win nwin;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Get_processor_name(procname,&namelen);
        

    printf("Hello World! process %d of %d on %s\n",rank,nprocs,procname);
    fflush(stdout);
    ABsize = 1024 *1024 *100 /sizeof(double);

    f_time = 0;
    printf("***************************\n");
    A = (double *)malloc(ABsize * sizeof(double));
    MPI_Win_create(A,ABsize*sizeof(double),sizeof(double),MPI_INFO_NULL,MPI_COMM_WORLD,&nwin);

    for(i=0;i<4;i++){
    if(0 != rank){
        printf("%d client rank:>%d<\n",i,rank);
        sprintf(buf," %d client:>%d<!\n",i,rank);
        B = (double *)malloc(ABsize * sizeof(double));
        s_time = MPI_Wtime();
        for(j=0;j<ABsize;j++){
            if(j<1){
                sleep(1);
            }
            B[j] = j+i;
        }

        if(i>0){ 
            MPI_Recv(buf,sizeof(buf),MPI_BYTE,0,0,MPI_COMM_WORLD,&status);
//            MPI_Win_unlock(rank,nwin);
        }
                
        sleep(1);
        B[0] = rank;

        printf("compute over!  time:%d \n",i);
//        MPI_Win_lock(MPI_LOCK_EXCLUSIVE,rank,0,nwin);
        memcpy(A,B,ABsize*sizeof(double));
//        MPI_Put(B,ABsize*sizeof(double),MPI_DOUBLE,1,0,ABsize*sizeof(double),MPI_DOUBLE,nwin);
//        MPI_Win_unlock(rank,nwin);
        printf("send begin!  time:%d \n",i);
        MPI_Send(buf,sizeof(buf),MPI_BYTE,0,0,MPI_COMM_WORLD);
//        MPI_Win_lock(MPI_LOCK_EXCLUSIVE,rank,0,nwin);
        if(i == 3){
//            MPI_Win_unlock(rank,nwin);
        }
        e_time = MPI_Wtime();
        printf("client:>%d<---%f---\n",rank,e_time-s_time);
        fflush(stdout);
        free(B);
    }else {
        int k;
        int source; 
        for(k=0;k<nprocs-1;k++){
        MPI_Recv(buf,sizeof(buf),MPI_BYTE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
        printf(">%d< %d I get it,msg:%s\n",rank,i,buf);
        
        source = status.MPI_SOURCE;
        s_time = MPI_Wtime();
        printf("pre lock source:%d!\n",source);
        fflush(stdout);
        //MPI_Win_lock(MPI_LOCK_SHARED,status.MPI_SOURCE,0,nwin);
        MPI_Win_lock(MPI_LOCK_SHARED,source,0,nwin);
        printf("lock!\n");
        fflush(stdout);
        MPI_Get(A,ABsize*sizeof(double),MPI_DOUBLE,source,0,ABsize*sizeof(double),MPI_DOUBLE,nwin);
//        MPI_Rget(A,ABsize*sizeof(double),MPI_DOUBLE,0,0,ABsize*sizeof(double),MPI_DOUBLE,nwin,&req);
//        MPI_Wait(&req,&winstatus);
//        MPI_Win_flush(0,nwin);
//        MPI_Win_unlock(status.MPI_SOURCE,nwin);
        MPI_Win_unlock(source,nwin);
        e_time = MPI_Wtime();
        f_time = e_time-s_time+f_time;
        for(j = 0;j<10;j++){
            printf("%f, ",A[j]);
        }
        printf("\nserver:>%d<---%f---\n",rank,e_time-s_time);

        MPI_Send(buf,sizeof(buf),MPI_BYTE,status.MPI_SOURCE,0,MPI_COMM_WORLD);
        fflush(stdout);
        }
    }
}

    printf("all time:%f\n",f_time);
    MPI_Barrier(MPI_COMM_WORLD);
    free(A);
    MPI_Win_free(&nwin);
    MPI_Finalize();

}
