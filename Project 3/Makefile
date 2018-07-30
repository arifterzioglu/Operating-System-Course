obj-m += module1.o



all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -g -Wall -o app app.c -lrt


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -fr *~ *.o app.c

