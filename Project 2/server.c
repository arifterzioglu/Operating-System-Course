#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#define SHAREDMEM_SIZE 999999999 /* bytes; large enough to hold the data */
#define NUM_ITEMS_TOPASS 50
#define sem_name "/arif_sem"
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
    struct shared_data * sdata_ptr;
};
sem_t* resArrMut[10];
sem_t* resArrEmpt[10];
sem_t* resArrFull[10];
void *runner(void* parameter);
int main (int argc, char *argv[])
{
    char sharedmem_name[128];
    char sem_state[128];
    sprintf(sem_state,"%s%s",argv[3],"_state");
    char input_name[128];
    strcpy(input_name, argv[2]);
    char sem_name1[128];
    sprintf(sem_name1, "%s%s",argv[3],"_reqMut");
    char sem_name2[128];
    sprintf(sem_name2, "%s%s",argv[3],"_reqEmp");
    char sem_name3[128];
    sprintf(sem_name3, "%s%s",argv[3],"_reqFull");


    char sem_res1[128];
    sprintf(sem_res1,"%s%s",argv[3],"_res1_");
    char sem_res2[128];
    sprintf(sem_res2,"%s%s",argv[3],"_res2_");
    char sem_res3[128];
    sprintf(sem_res3,"%s%s",argv[3],"_res3_");

    int fd;
    pthread_t tid[10];
    pthread_attr_t attr[10];

    void *shm_start;
    struct shared_data *sdata_ptr;
    struct stat sbuf;

    //shared memory allocation
    strcpy(sharedmem_name, argv[1]);
    shm_unlink(sharedmem_name);

    fd = shm_open(sharedmem_name, O_RDWR | O_CREAT, 0660);

    if (fd < 0)
    {
        exit (1);
    }


    ftruncate(fd, SHAREDMEM_SIZE);
    fstat(fd, &sbuf);

    shm_start = mmap(NULL,sbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fd,0);


    if (shm_start < 0)
    {
        exit (1);
    }

    close(fd);

    sdata_ptr = (struct shared_data *) shm_start;

    //initilizations
    for(int x = 0; x<10;x++)
    {
        sdata_ptr->queue_state[x] = 0;
        sdata_ptr->res_queue[0].result[x] = 0;
        sdata_ptr->res_queue[1].result[x] = 0;
        sdata_ptr->res_queue[2].result[x] = 0;
        sdata_ptr->res_queue[3].result[x] = 0;
        sdata_ptr->res_queue[4].result[x] = 0;
        sdata_ptr->res_queue[5].result[x] = 0;
        sdata_ptr->res_queue[6].result[x] = 0;
        sdata_ptr->res_queue[7].result[x] = 0;
        sdata_ptr->res_queue[8].result[x] = 0;
        sdata_ptr->res_queue[9].result[x] = 0;

    }

    sdata_ptr->inReq = 0;
    sdata_ptr->outReq = 0;

    //semaphores creations
    sem_unlink(sem_state);
    sem_open(sem_state, O_RDWR | O_CREAT,0660,1);


    sem_unlink(sem_name1);
    sem_unlink(sem_name2);
    sem_unlink(sem_name3);


    sem_t* reqMut = sem_open(sem_name1, O_RDWR | O_CREAT, 0660, 1 );
    sem_t* reqEmpt = sem_open(sem_name2, O_RDWR | O_CREAT, 0660, 10 );
    sem_t* reqFull =sem_open(sem_name3, O_RDWR | O_CREAT, 0660, 0 );

    char sep1[128];
    char sep2[128];
    char sep3[128];

    for(int i = 0; i<10;i++)
    {
        sprintf(sep1, "%s%d",sem_res1,i);
        sprintf(sep2, "%s%d",sem_res2,i);
        sprintf(sep3, "%s%d",sem_res3,i);
        sem_unlink(sep1);
        sem_unlink(sep2);
        sem_unlink(sep3);
        resArrMut[i] = sem_open(sep1, O_RDWR | O_CREAT, 0660, 1 );
        resArrEmpt[i] = sem_open(sep2, O_RDWR | O_CREAT, 0660, 100 );
        resArrFull[i] = sem_open(sep3, O_RDWR | O_CREAT, 0660, 0 );


    }

    //request queue
    char keyword3[128];
    int queue_index3=-8;
    do
    {

        //printf("%d\n",queue_index3);

        sem_wait(reqFull);
        sem_wait(reqMut);

        //critical section

        queue_index3 = sdata_ptr->req_queue[sdata_ptr->outReq].queue_index;
        strcpy(keyword3, sdata_ptr->req_queue[sdata_ptr->outReq].keyword);
        sdata_ptr->outReq = (sdata_ptr->outReq+1) %10;



        sem_post(reqMut);
        sem_post(reqEmpt);



		struct param * params = malloc(sizeof(struct param));
		strcpy(params->input_name2, input_name);
		strcpy(params->keyword, keyword3);
		params->q_index = queue_index3;
		params->sdata_ptr = sdata_ptr;


        //thread allocation


        pthread_attr_init(&(attr[queue_index3]));
        pthread_create(&(tid[queue_index3]),&(attr[queue_index3]),&runner,(void*)params);

   		queue_index3 = -8;




    }
    while(1);

}
//runner function
void *runner(void* parameter)
{
	struct param * par = parameter;
    int line_num = 1;
    char line[1024];
    char word[128];
    char address[128];
    strcpy(word, par->keyword);
    strcpy(address, par->input_name2);
    int index = par->q_index;

    FILE *filePointer = fopen(address,"r");
    while(fgets(line, 1023, filePointer))
    {
        if(strstr(line,word) != NULL)
        {
            sem_wait(resArrEmpt[index]);
            sem_wait(resArrMut[index]);
            //critical section

            par->sdata_ptr->res_queue[index].result[par->sdata_ptr->res_queue[index].in] = line_num;
            par->sdata_ptr->res_queue[index].in = (par->sdata_ptr->res_queue[index].in+1) % 100;

            sem_post(resArrMut[index]);
            sem_post(resArrFull[index]);
            //sleep(1);


        }
        line_num++;

    }
    sem_wait(resArrEmpt[index]);
    sem_wait(resArrMut[index]);
    //critical section

    par->sdata_ptr->res_queue[index].result[par->sdata_ptr->res_queue[index].in] = -1;
    par->sdata_ptr->res_queue[index].in = (par->sdata_ptr->res_queue[index].in+1) % 100;

    sem_post(resArrMut[index]);
    sem_post(resArrFull[index]);
    pthread_exit(0);

}

