#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>

/****************************
 *JiHan  2018 .02.07
 *send and recv of MPI
 ***************************/

void main(int argc,char *argv[]){
    int rank,nprocs;
    int namelen,i,j;
    int reqest;
    char procname[MPI_MAX_PROCESSOR_NAME];
    char buf[256];
    double s_time,e_time,f_time = 0;
    MPI_Request req;
    MPI_Status status,winstatus;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Get_processor_name(procname,&namelen);
        

    printf("Hello World! process %d of %d on %s\n",rank,nprocs,procname);
    fflush(stdout);
    
    for(i=0;i<5;i++){
    printf("----------------loop:%d---------------\n",i);
    if(0 == rank){
        sleep(2);
        sprintf(buf," 1 client num:%d!",i);
        printf(">%d< send 1 begin\n",rank);
        fflush(stdout);
        MPI_Send(buf,sizeof(buf),MPI_BYTE,1,0,MPI_COMM_WORLD);
        sleep(2);
        sprintf(buf," 1 client num:%d!",i);
        printf(">%d< send 2 begin\n",rank);
        fflush(stdout);
        MPI_Send(buf,sizeof(buf),MPI_BYTE,1,0,MPI_COMM_WORLD);
        printf(">%d< recv begin\n",rank);
        fflush(stdout);
        sleep(2);
        MPI_Recv(&reqest,sizeof(reqest),MPI_INT,1,0,MPI_COMM_WORLD,&status);
        printf(">%d< recv over msg:%d\n",rank,reqest);
        fflush(stdout);
    }else if(1 == rank){
        printf(">%d< Irecv begin!\n",rank);
        fflush(stdout);
        MPI_Irecv(buf,sizeof(buf),MPI_BYTE,0,0,MPI_COMM_WORLD,&req);
        printf(">%d< Irecv over!\n",rank);
        fflush(stdout);
        MPI_Wait(&req,&status);
        printf(">%d< Irecv msg:%s\n",rank,buf);
        fflush(stdout);
        MPI_Recv(buf,sizeof(buf),MPI_BYTE,0,0,MPI_COMM_WORLD,&status);
        printf(">%d< recv over! and msg:%s\n",rank,buf);
        fflush(stdout);
        printf(">%d< Ssend begin!\n",rank);
        fflush(stdout);
        reqest = rank;
        MPI_Ssend(&reqest,sizeof(reqest),MPI_INT,0,0,MPI_COMM_WORLD);
        printf(">%d< Ssend over!\n",rank);
        fflush(stdout);
    }
    }

    MPI_Finalize();

}
