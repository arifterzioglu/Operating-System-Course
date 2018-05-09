#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#define SHAREDMEM_SIZE 999999999 /* bytes; large enough to hold the data */
#define NUM_ITEMS_TOPASS 50
struct req_data
{
        int queue_index;
        char keyword[128];
};
struct resultQueue
{
    int result[100];
    int in;
    int out;
};
struct shared_data
{

    struct req_data  req_queue[10];
    int inReq;
    int outReq;
	struct resultQueue res_queue[10];



    int queue_state[10];

};

struct param
{
    char input_name2[128];
    char keyword[128];
    int q_index;
    struct shared_data *sdata_ptr;
};
int main (int argc, char *argv[])
{

    char sharedmem_name[128];

    int fd;
    void *shm_start;
    char err[120] = "too many clients started";
    struct shared_data *sdata_ptr;
    struct stat sbuf;

    //shared memory allocation
    strcpy(sharedmem_name, argv[1]);
    fd = shm_open(sharedmem_name, O_RDWR, 0660);
    if (fd < 0)
    {
        exit (1);
    }


    fstat(fd, &sbuf);

    shm_start = mmap(NULL, sbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shm_start < 0) {
        exit (1);
    }

    close(fd);


    sdata_ptr = (struct shared_data *) shm_start;


    //semaphores are opened
    char sem_state[128];
	sprintf(sem_state,"%s%s",argv[3],"_state");
    sem_t * semState;
    semState = sem_open(sem_state, O_RDWR);


    char sem_name1[128];
    sprintf(sem_name1, "%s%s",argv[3],"_reqMut");
    char sem_name2[128];
    sprintf(sem_name2, "%s%s",argv[3],"_reqEmp");
    char sem_name3[128];
    sprintf(sem_name3, "%s%s",argv[3],"_reqFull");
    sem_t * reqMut = sem_open(sem_name1,O_RDWR );
    sem_t * reqEmpt = sem_open(sem_name2, O_RDWR );
    sem_t * reqFull =sem_open(sem_name3, O_RDWR );

    char sem_res1[128];
    sprintf(sem_res1,"%s%s",argv[3],"_res1_");
    char sem_res2[128];
    sprintf(sem_res2,"%s%s",argv[3],"_res2_");
    char sem_res3[128];
    sprintf(sem_res3,"%s%s",argv[3],"_res3_");
    char sep1[128];
    char sep2[128];
    char sep3[128];
    sem_t* resArrMut[10];
    sem_t* resArrEmpt[10];
    sem_t* resArrFull[10];

    for(int i = 0; i<10;i++)
    {
        sprintf(sep1, "%s%d",sem_res1,i);
        sprintf(sep2, "%s%d",sem_res2,i);
        sprintf(sep3, "%s%d",sem_res3,i);
        resArrMut[i] = sem_open(sep1, O_RDWR );
        resArrEmpt[i] = sem_open(sep2, O_RDWR );
        resArrFull[i] = sem_open(sep3, O_RDWR );


    }


    int flag = -9;


    //state queue

    sem_wait(semState);


    //critical section for queueu state

    for(int i  = 0; i< 10 ; i++)
    {
        if(sdata_ptr->queue_state[i]==0)
        {

            sdata_ptr->queue_state[i]=1;
            flag = i;
            break;

        }


    }
    sem_post(semState);
	//printf("%d\n",flag);

    if(flag ==-9)
    {
        printf("%s\n",err);
        exit(1);

    }

    //create request
    sem_wait(reqEmpt);
    sem_wait(reqMut);

    //Critical section

    strcpy(sdata_ptr->req_queue[sdata_ptr->inReq].keyword, argv[2]);
    sdata_ptr->req_queue[sdata_ptr->inReq].queue_index = flag;
    sdata_ptr->inReq = (sdata_ptr->inReq+1) %10;


    //result queue
    sem_post(reqMut);
    sem_post(reqFull);

    int x = -89;
    do
    {
        sem_wait(resArrFull[flag]);
        sem_wait(resArrMut[flag]);

        //critical section
        x= sdata_ptr->res_queue[flag].result[sdata_ptr->res_queue[flag].out];
        sdata_ptr->res_queue[flag].out = (sdata_ptr->res_queue[flag].out +1) % 100;


        sem_post(resArrMut[flag]);
        sem_post(resArrEmpt[flag]);

	if(x!=-1)
        	printf("%d\n",x);


    }
    while(x !=-1);
   sem_wait(semState);


    //critical section for queueu state

    sdata_ptr->queue_state[flag]=0;
    sem_post(semState);

    //close semaphores
    sem_close(semState);
    sem_close(reqMut);
    sem_close(reqEmpt);
    sem_close(reqFull);

    for(int i = 0; i<10;i++)
    {
        sprintf(sep1, "%s%d",sem_res1,i);
        sprintf(sep2, "%s%d",sem_res2,i);
        sprintf(sep3, "%s%d",sem_res3,i);
        sem_close(resArrMut[i]);
        sem_close(resArrEmpt[i]);
        sem_close(resArrFull[i]);

    }
    exit(1);

















}
