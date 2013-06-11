obj-m := chardev.o

KDIR := /lib/modules/$(shell uname -r)/build
#KDIR := /usr/src/linux-3.1.4/
#KDIR := /lib/modules/3.0.0-12-generic/build
PWD := $(shell pwd)


default:	
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules	
remov:
	sudo rmmod chardev
insert: default
	sudo insmod ./chardev.ko
reinsert: remov insert

clean: 
	rm *.o
	rm *.ko
	

