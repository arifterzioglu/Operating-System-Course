/*Arif Can Terzioğlu 21302061
Kübra Nur Güzel 21400946*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
int currentTimeFCFS = 0;
int reqSize;
int headFCFS = 1;
int headSSTF = 1;
int headLook = 1;
int headClook = 1;
int FCFStime = 0;
int SSTFtime = 0;
int LOOKtime = 0;
int CLOOKtime = 0;
double FCFSmean = 0;
int iniSize = 0;
int reqSize = 0;

int absVal(int x)
{
    if(x<0)
    {
        return x*(-1);
    }
    else
    {
        return x;
    }


}
struct input
{
    int arrivalTime;
    int cylinder;
    struct input *next;
};

struct Queue
{
    struct input *front, *rear;
};

struct input* newNode(int arrival, int cylinder)
{
    struct input *temp = (struct input*)malloc(sizeof(struct input));
    temp->arrivalTime = arrival;
    temp->cylinder = cylinder;
    temp->next = NULL;
    return temp;
}

struct Queue *createQueue()
{
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a key k to q
void enQueue(struct Queue *q, int arrival, int cylinder)
{

    struct input *temp = newNode(arrival,cylinder);

    if (q->rear == NULL)
    {
       q->front = q->rear = temp;
        iniSize++;
       return;
    }

    q->rear->next = temp;
    q->rear = temp;
    iniSize++;
}
void enQueue2(struct Queue *q, int arrival, int cylinder)
{
    // Create a new LL node
    struct input *temp = newNode(arrival,cylinder);

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL)
    {
       q->front = q->rear = temp;
       reqSize++;
       return;
    }

    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
    reqSize++;
}
// Function to remove a key from given queue q
struct input *deQueue(struct Queue *q)
{
    // If queue is empty, return NULL.
    if (q->front == NULL)
       return NULL;

    struct input *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
       q->rear = NULL;

    return temp;
}
struct input *deQueue2(struct Queue *q)
{
    if (q->front == NULL)
       return NULL;

    struct input *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
       q->rear = NULL;

    reqSize--;
    return temp;
}
int  deQueueWithoutDelete(struct Queue *q)
{
    // If queue is empty, return 0.
    if (q->front == NULL)
       return 0;

    struct input *temp = q->front;

    return temp->arrivalTime;
}

//FCFS calculations
void FCFS(struct input *tmp)
{
    //FCFSmean = FCFSmean+(currentTimeFCFS - tmp->arrivalTime);
    int diff = absVal(tmp->cylinder - headFCFS);
    FCFStime = FCFStime + diff;
    headFCFS = tmp->cylinder;
    currentTimeFCFS = currentTimeFCFS + diff;
}
//SSTF Calculations
void SSTF(struct input *tmp)
{
    //FCFSmean = FCFSmean+(currentTimeFCFS - tmp->arrivalTime);
    int diff = absVal(tmp->cylinder - headSSTF);
    SSTFtime = SSTFtime + diff;
    headSSTF = tmp->cylinder;
    currentTimeFCFS = currentTimeFCFS + diff;
}
//LOOK calculations
void LOOK(struct input *tmp)
{
    int diff = absVal(tmp->cylinder - headLook);
    LOOKtime = LOOKtime + diff;
    headLook = tmp->cylinder;
    currentTimeFCFS = currentTimeFCFS + diff;
}
//CLOOK calculations
void CLOOK(struct input *tmp)
{
    int diff = absVal(tmp->cylinder - headClook);
    CLOOKtime = CLOOKtime + diff;
    headClook = tmp->cylinder;
    currentTimeFCFS = currentTimeFCFS + diff;
}
void print_list(struct input * head)
{
    struct input *current = head;

    while (current != NULL)
    {
        printf("\n%d %d\n", current->arrivalTime, current->cylinder);
        current = current->next;
    }
}
void calculateSD(int data[], int n, int time, int type)
{
    double mean=0.0, sum_deviation=0.0;
    int i;
    for(i=0; i<n;i++)
    {
        mean+=data[i];
    }
    mean=mean/n;
    for(i=0; i<n;++i)
    {
        sum_deviation+=(data[i]-mean)*(data[i]-mean);

    }
    if(type ==1)
    {
        printf("FCFS : %d %2f %2f\n",time,mean,sqrt(sum_deviation/(n-1)));

    }
    else if(type == 2)
    {
        printf("SSTF : %d %2f %2f\n",time,mean,sqrt(sum_deviation/(n-1)));
    }
    else if(type == 3)
    {
        printf("LOOK : %d %2f %2f\n",time,mean,sqrt(sum_deviation/(n-1)));

    }
    else
    {
        printf("CLOOK: %d %2f %2f\n",time,mean,sqrt(sum_deviation/(n-1)));
    }
}
int findNextHigher(struct input * arr2[], int arr[], int lookIndex)
{
    int minDif = 999999999;
    int returnIndex = -1;
    for(int i = 0; i<lookIndex;i++)
    {

        if(arr[i]!=1 && (arr2[i]->cylinder) >= headLook)
        {
            int tmp = arr2[i]->cylinder - headLook;

            if(tmp < minDif)
            {
                minDif=tmp;
                returnIndex = i;
            }
        }
    }

    return returnIndex;
}
int findNextHigher2(struct input * arr2[], int arr[], int ClookIndex)
{
    int minDif = 999999999;
    int returnIndex = -1;
    for(int i = 0; i<ClookIndex;i++)
    {

        if(arr[i]!=1 && (arr2[i]->cylinder) >= headClook)
        {
            int tmp = arr2[i]->cylinder - headClook;

            if(tmp < minDif)
            {
                minDif=tmp;
                returnIndex = i;
            }
        }
    }

    return returnIndex;
}
int findNextLower(struct input * arr2[], int arr[], int lookIndex)
{
    int minDif = 999999999;
    int returnIndex = -1;
    for(int i = 0; i<lookIndex;i++)
    {

        if(arr[i]!=1 && (arr2[i]->cylinder) <= headLook)
        {
            int tmp = headLook-arr2[i]->cylinder;

            if(tmp < minDif)
            {
                minDif=tmp;
                returnIndex = i;
            }
        }
    }

    return returnIndex;
}
int findLowest(struct input * arr2[], int arr[], int ClookIndex)
{

    int maxDif = -999999999;
    int returnIndex = -1;
    for(int i = 0; i<ClookIndex;i++)
    {

        if(arr[i]!=1 && (arr2[i]->cylinder) <= headClook)
        {
            int tmp = headClook-arr2[i]->cylinder;

            if(tmp > maxDif)
            {
                maxDif=tmp;
                returnIndex = i;
            }
        }
    }

    return returnIndex;
}
int findMin(struct input * arr[], int head,int size)
{
    int minimum = absVal(arr[0]->cylinder - head);
    int loc = 0;

    for ( int c = 1 ; c < size ; c++ )
    {
        if ( (absVal(arr[c]->cylinder - head)) < minimum )
        {
           minimum = (absVal(arr[c]->cylinder - head));
           loc = c;
        }
    }

    return loc;


}
int main (int argc, char *argv[])
{
    //get the input file
	char address[256];
    strcpy(address, argv[2]);
    char line[1024];
    FILE *filePointer = fopen(address,"r");
    int arriv = 1;
    int arrival = 0;
    int cylinder = 0;
    struct Queue *insQueue = createQueue();
    int count = 0;
    while(fgets(line, 148, filePointer))
    {
        char *p = strtok(line, " ");
        while(p !=NULL)
        {
            if(arriv ==1)
            {
                arrival = atoi(p);
                arriv = 0;
            }
            else
            {
                cylinder = atoi(p);
                arriv = 1;
            }
            p = strtok(NULL," ");
            count++;
            if(count == 2)
            {
                if(cylinder != 0)
                {
                    enQueue(insQueue,arrival,cylinder);
                }

                count = 0;

            }
        }
    }
    fclose(filePointer);

    //FCFS
    int dataFCFS[iniSize];
    int index = 0;
    struct Queue *reqQueue = createQueue();
    currentTimeFCFS = deQueueWithoutDelete(insQueue);
    int x = 0;
    while(1)
    {

        while(x !=iniSize && currentTimeFCFS >= deQueueWithoutDelete(insQueue))
        {
            struct input *n = deQueue(insQueue);
            enQueue2(reqQueue, n->arrivalTime,n->cylinder);
            x++;
        }

        if(reqSize == 0)
        {
            if(x == iniSize)
            {
                break;
            }
            currentTimeFCFS = currentTimeFCFS+1;
        }
        else
        {
            struct input *tmp = deQueue2(reqQueue);
            if(index == 0)
                dataFCFS[index]=0;
            else
                dataFCFS[index]= (currentTimeFCFS - tmp->arrivalTime);
            index++;
            FCFS(tmp);
        }

    }

    calculateSD(dataFCFS,iniSize, FCFStime,1);

    ////////////////////////////////////////////////////////////////
    filePointer = fopen(address,"r");
    iniSize = 0;
    while(fgets(line, 148, filePointer))
    {
        char *p = strtok(line, " ");
        while(p !=NULL)
        {
            if(arriv ==1)
            {
                arrival = atoi(p);
                arriv = 0;
            }
            else
            {
                cylinder = atoi(p);
                arriv = 1;
            }
            p = strtok(NULL," ");
            count++;
            if(count == 2)
            {
                if(cylinder != 0)
                {
                    enQueue(insQueue,arrival,cylinder);
                }

                count = 0;

            }
        }
    }
    fclose(filePointer);


    currentTimeFCFS = 0;

    //SSTF
    x = 0;
    int dataSSTF[iniSize];
    int index2= 0;
    struct input * arr[iniSize];
    index = 0;
    while(1)
    {

        while(x !=iniSize && currentTimeFCFS >= deQueueWithoutDelete(insQueue))
        {
            struct input *n = deQueue(insQueue);
            arr[index] = n;
            index++;
            x++;
            reqSize++;
        }

        if(reqSize == 0)
        {
            if(x == iniSize)
            {
                break;
            }
            currentTimeFCFS = currentTimeFCFS+1;
        }
        else
        {
            int l = findMin(arr,headSSTF,index);
            if(index2 == 0)
                dataSSTF[index2] = 0;
            else
                dataSSTF[index2] = currentTimeFCFS - arr[l]->arrivalTime;
            SSTF(arr[l]);
            index2++;
            reqSize--;
            arr[l]->cylinder = 9999999;
        }

    }

    calculateSD(dataSSTF,iniSize, SSTFtime,2);

    ////////////////////////////////////////////////////////

    filePointer = fopen(address,"r");
    iniSize = 0;
    while(fgets(line, 148, filePointer))
    {
        char *p = strtok(line, " ");
        while(p !=NULL)
        {
            if(arriv ==1)
            {
                arrival = atoi(p);
                arriv = 0;
            }
            else
            {
                cylinder = atoi(p);
                arriv = 1;
            }
            p = strtok(NULL," ");
            count++;
            if(count == 2)
            {
                if(cylinder != 0 )
                {
                    enQueue(insQueue,arrival,cylinder);
                }

                count = 0;

            }
        }
    }
    fclose(filePointer);

    //LOOK
    currentTimeFCFS = 0;
    struct input * arr2[iniSize];
    int used[iniSize];
    for(int i = 0; i<iniSize;i++)
    {

        used[i] = 0;
    }
    int lookIndex = 0;
    int direction =0;
    int dataLook[iniSize];
    index2=0;
    x=0;
    while(1)
    {

        while(x !=iniSize && currentTimeFCFS >= deQueueWithoutDelete(insQueue))
        {
            struct input *n = deQueue(insQueue);
            arr2[lookIndex] = n;
            lookIndex++;
            x++;
            reqSize++;
        }

        if(reqSize == 0)
        {
            if(x == iniSize)
            {
                break;
            }
            currentTimeFCFS = currentTimeFCFS+1;
        }
        else
        {
            int l;
            if(direction==0)
            {
                l = findNextHigher(arr2,used,lookIndex);
                if(l==-1)
                {
                    direction = 1;
                }
                else
                {
                    used[l] = 1;
                    if(index2 == 0)
                        dataLook[index2] = 0;
                    else
                        dataLook[index2] = currentTimeFCFS - arr[l]->arrivalTime;
                    LOOK(arr2[l]);
                    reqSize--;
                    index2++;


                }
            }
            else if(direction ==1)
            {

                l = findNextLower(arr2,used,lookIndex);
                if(l==-1)
                {
                    direction = 0;
                }
                else
                {
                    used[l] = 1;
                    if(index2 == 0)
                        dataLook[index2] = 0;
                    else
                        dataLook[index2] = currentTimeFCFS - arr[l]->arrivalTime;
                    LOOK(arr2[l]);
                    reqSize--;
                    index2++;

                }


            }


        }

    }
    calculateSD(dataLook,iniSize,LOOKtime,3);



    ///////////////////////////////////////////////////////////////////////





    filePointer = fopen(address,"r");
    iniSize = 0;
    while(fgets(line, 148, filePointer))
    {
        char *p = strtok(line, " ");
        while(p !=NULL)
        {
            if(arriv ==1)
            {
                arrival = atoi(p);
                arriv = 0;
            }
            else
            {
                cylinder = atoi(p);
                arriv = 1;
            }
            p = strtok(NULL," ");
            count++;
            if(count == 2)
            {
                if(cylinder != 0 )
                {
                    enQueue(insQueue,arrival,cylinder);
                }

                count = 0;

            }
        }
    }
    fclose(filePointer);





    //CLOOK
    currentTimeFCFS = 0;
    struct input * arr3[iniSize];
    for(int i = 0; i<iniSize;i++)
    {

        used[i] = 0;
    }
    int CLOOKIndex = 0;
    direction =0;
    int dataCLook[iniSize];
    index2=0;
    x=0;

    int change = 0;
    int changeAmount = 0;
    while(1)
    {

        while(x !=iniSize && currentTimeFCFS >= deQueueWithoutDelete(insQueue))
        {
            struct input *n = deQueue(insQueue);
            arr3[CLOOKIndex] = n;
            CLOOKIndex++;
            x++;
            reqSize++;
        }

        if(reqSize == 0)
        {
            if(x == iniSize)
            {
                break;
            }
            currentTimeFCFS = currentTimeFCFS+1;
        }
        else
        {
            int l;
            if(direction==0)
            {
                l = findNextHigher2(arr3,used,CLOOKIndex);

                if(l==-1)
                {
                    direction = 1;
                }
                else
                {
                    used[l] = 1;
                    if(index2 == 0)
                    {
                        dataCLook[index2] = 0;
                    }
                    else
                    {
                        dataCLook[index2] = currentTimeFCFS - (arr3[l]->arrivalTime);
                    }
                    CLOOK(arr3[l]);
                    reqSize--;
                    index2++;
                    if(change ==1)
                    {
                        currentTimeFCFS = currentTimeFCFS+changeAmount;
                        change=0;

                    }

                }
            }
            else if(direction ==1)
            {
                l = findLowest(arr3,used,CLOOKIndex);

                if(l != -1)
                {
                    direction = 0;
                    CLOOKtime = CLOOKtime + absVal(arr3[l]->cylinder - headClook);
                    changeAmount=0;
                    changeAmount = changeAmount+absVal(arr3[l]->cylinder - headClook);
                    change=1;
                    headClook = arr3[l]->cylinder;

                }

            }


        }

    }
   calculateSD(dataCLook,iniSize,CLOOKtime,4);


}




