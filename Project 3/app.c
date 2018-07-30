#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int factorial(int n)
{
    if (n < 1)
        return 1;

    int x = n*factorial(n-1);

    return x;
}


int main (int argc, char *argv[])
{
 	printf("%d\n",getpid());
    int g;
    int o;
    char *str;
    char *str2;
    char *str3;
    char *str4;
    char *str5;
    char *str6;
    char *str7;

    printf("1 for memory allocation, 2 for  stack allocation\n");
    scanf("%d",&g);

    if(g==1)
    {

        str = (char *) malloc(60000);
        str2 = (char *) malloc(60000);
        str3 = (char *) malloc(60000);
        str4 = (char *) malloc(60000);
        str5 = (char *) malloc(60000);
        str6 = (char *) malloc(60000);
        str7 = (char *) malloc(60000);
        printf("memory  allocated\n");


    }
    else
    {
        int x = factorial(20000);
        printf("%d\n",x);
    }


    printf("1 for memory deallocation, 2 for  exit\n");
    scanf("%d",&o);
    if(o==1)
    {
        free(str);
        free(str2);
        free(str3);
        free(str4);
        free(str5);
        free(str6);
        free(str7);
        printf("memory deallocated\n");

    }
    else
    {
        exit(1);



    }

    printf("1 for exit\n");
    scanf("%d",&o);
    if(o == 1)
    {
        exit(1);
    }

    return 1;







}
