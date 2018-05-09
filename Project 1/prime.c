#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <fcntl.h>
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

int main(int argc, char *argv[])
{
	pid_t pid;
	int N =atoi(argv[1]);
	int M =atoi(argv[2]);
	int printP = 0;
	int filter  =1;
	int h = 2;
	int cond = 1;
	int val;
	int fd[2*(M+2)];
	//create pipes
	for(int i =0; i < M+2; i++)
	{
		pipe(fd+i*2);

	}

    fcntl(fd[2*M],F_SETFL,O_NONBLOCK);
    fcntl(fd[1],F_SETFL,O_NONBLOCK);

    //child processes
	for(int x =0; x < M; x++)
	{
		pid = fork();
		if(pid == 0) /* child*/
		{
            int prime = 0;
			int value = 0;
			do
			{
                while(read(fd[2*x], &value, sizeof(value))>0)
                {
                    if(value==-8)
                    {
                        int cnt = -5;
                        write(fd[2*x+3],&value,sizeof(value));
                        if(x==M-1)
                                write(fd[2*(M+1)+1],&value,sizeof(value));

                        exit(1);
                    }

                    if( value == -1)
                    {
                        prime = 0;

                        write(fd[2*x+3],&value,sizeof(value));



                    }
                    else
                    {
                        if( value!=0 && prime == 0)
                        {
                            prime = value;
                            write(fd[2*(M+1)+1],&value,sizeof(value));

                        }

                        if(value!=0 && value % prime !=0)
                        {

                            write(fd[2*x+3],&value,sizeof(value));

                        }
                    }
                }

			}

			while(1);



		}
	}

	pid = fork();
	if(pid == 0) /* Print child*/
	{
		while(read(fd[2*(M+1)], &printP,sizeof(printP))>0)
		{
            if(printP == -8)
            {
                exit(1);
            }

			printf("%d\n", printP);

		}
	}
	if(pid != 0) /*paret*/
	{
		struct Queue *q = createQueue();
		int  h = 2;

		while(h<=N)
		{

			enQueue(q,h);
			h++;

		}

        enQueue(q,-1);
        int nb = 0;


        while(1)
        {
            if(cond==1)
            {
                if(size==1)
                {
                    nb = -8;
                    write(fd[1],&nb,sizeof(nb));
                    //fprintf(stderr,"waiting...");
                    wait(NULL);

                    for(int i =0; i < M+2; i++)
                    {
                        close(fd[2*i]);
                        close(fd[2*i+1]);

                    }
                    break;
                }

                while(nb!=-1)
                {
                    int sendCont=-9;

                    nb = deQueueNonDelete(q);

                    sendCont = write(fd[1],&nb,sizeof(nb));

                    if(sendCont>0)
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

            if(read(fd[2*M],&val,sizeof(val))>0 && val<=N)
            {

                if(val!=-1  )
                {
                    enQueue(q,val);

                }
                else if(val==-1)
                {
                    enQueue(q,val);
                    nb=0;
                    cond=1;

                }
            }
        }

	}





	return 0;


}

