#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#define MQNAME_PREFIX "/ar_if_qeueu"
#define ITEMSIZE sizeof(int)*6
#define INVALID_INT -1000
int size = 0;
struct Node
{

	int key;
	struct Node *next;
};

struct Queue
{
	struct Node *front, *rear;

};

struct Node* newNode(int n)
{
	struct Node *temp = (struct Node*)malloc(sizeof(struct Node));
	temp->key = n;
	temp->next= NULL;
	return temp;
};
struct Queue *createQueue()
{
	struct Queue *q = (struct Queue*)malloc(sizeof(struct Node));
	q->front = q->rear = NULL;
	return q;
};
int isEmpty(struct Queue *q)
{
    if(q->front==NULL)
    {
        return 1;
    }
    else
        return 0;

}
void enQueue(struct Queue *q, int k)
{


        struct Node *temp = newNode(k);


        if (q->rear == NULL)
        {
            q->front = q->rear = temp;
            size++;
            return;
        }


        q->rear->next = temp;
        q->rear = temp;
        size++;


}


struct Node *deQueue(struct Queue *q)
{

    if (q->front == NULL)
       return NULL;


    struct Node *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
       q->rear = NULL;



	size--;
    return temp;
}
int deQueueNonDelete(struct Queue *q)
{

    if (q->front == NULL)
       return 0;


    struct Node *temp = q->front;

    return temp->key;
}

int topQueue(struct Queue *q)
{
    if (q->front == NULL)
        return -9;

    return q->front->key;
}


struct item
{
    int key;
};
int main(int argc, char *argv[])
{

	pid_t pid;
	int M = atoi(argv[2]);
	int N= atoi(argv[1]);
	int x = 1;
	int n = 0;
	mqd_t mq[M+2];
	int cond = 1;
	char mq_name[128];


	for(int i = 0; i<M+2; i++)
	{
        int ch = mq_close(mq[i]);
		sprintf(mq_name, "%s%d",MQNAME_PREFIX,i);
		mq_unlink(mq_name);
	}

	for(int i = 1; i<M+2; ++i)
	{
            sprintf(mq_name, "%s%d",MQNAME_PREFIX,i);
            mq[i]= mq_open(mq_name, O_RDWR|O_CREAT,0666,NULL);
	}
    sprintf(mq_name, "%s%d",MQNAME_PREFIX,0);
    mq[0]= mq_open(mq_name, O_RDWR|O_CREAT|O_NONBLOCK,0666,NULL);



	for(int y =0; y < M; y++)
	{

        pid = fork();
		if(pid == 0) /* child*/
		{
            char *buffer;
			struct mq_attr mq_attr;
			int buflen;
			mq_getattr(mq[y],&mq_attr);
			buflen = mq_attr.mq_msgsize;
			buffer = (char *) malloc(buflen);
			int prime = 0;

			do
			{
                while(mq_receive(mq[y], buffer,buflen,NULL) != -1)
                {
                    x= *((int *)buffer);
                    if(x==-8)
                    {
                        mq_send(mq[y+1],(char *) &x,ITEMSIZE,0);
                        if(y==M-1)
                            mq_send(mq[M+1],(char *) &x,ITEMSIZE,0);
                        exit(1);
                    }

                    if( x == -1)
                    {
                        prime = 0;
                        mq_send(mq[y+1],(char *) &x,ITEMSIZE,0);
                    }
                    else
                    {
                        if(prime == 0)
                        {
                            prime = x;
                            mq_send(mq[M+1],(char *) &x,ITEMSIZE,0);

                        }

                        else if( x % prime !=0)
                        {
                            mq_send(mq[y+1],(char *) &x,ITEMSIZE,0);
                        }
                    }

                }
			}
			while(1);





        }

	}



	pid = fork();
	if(pid == 0) /* child*/
	{
        char *buffer;
        struct mq_attr mq_attr;
        int buflen;
        mq_getattr(mq[M+1],&mq_attr);
        buflen = mq_attr.mq_msgsize;
        buffer = (char *) malloc(buflen);
		while(mq_receive(mq[M+1], buffer,buflen,NULL) >0)
		{
            x= *((int *)buffer);
            if(x==-8)
                exit(1);
			printf("%d\n", x);

		}
	}



	//Main Process

		if(pid !=0)
		{

            char *buffer;
            struct mq_attr mq_attr;
            int buflen;
            mq_getattr(mq[0],&mq_attr);
            buflen = mq_attr.mq_msgsize;
            buffer = (char *) malloc(buflen);

            struct Queue *q = createQueue();
            int  h = 2;

            while(h<=N)
            {

                enQueue(q,h);
                h++;

            }
            enQueue(q,-1);
            int prev = -8;
            int nb = 0;

            while(1)
            {
                if(cond==1)
                {
                    if(size==1)
                    {
                        int term=-8;


                        mq_send(mq[0],(char *) &term,ITEMSIZE,0);
                        wait(NULL);
                        for(int i = 0; i<M+2; i++)
                        {

                            mq_close(mq[i]);

                            sprintf(mq_name, "%s%d",MQNAME_PREFIX,i);
                            mq_unlink(mq_name);


                        }

                        break;

                    }
                    while(nb!=-1)
                    {
                        int sendCont=-9;

                        nb = deQueueNonDelete(q);

                        sendCont = mq_send(mq[0],(char *) &nb,ITEMSIZE,0);


                        if(sendCont==0)
                        {

                            deQueue(q);
                            if(nb==-1)
                            {
                                cond=0;
                            }

                        }
                        else
                        {
                            nb=0;
                            break;

                        }


                    }



                }
                if(mq_receive(mq[M], buffer,buflen,NULL) >0)
                {
                    x= *((int *)buffer);


                    if(x!=-1)
                    {
                        enQueue(q,x);
                    }
                    else if(x==-1)
                    {
                        enQueue(q,x);
                        nb=0;
                        cond=1;
                    }


                }


            }


		}








	return 0;
}
