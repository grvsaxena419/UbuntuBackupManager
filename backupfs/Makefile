obj-m := wrapfs.o

KDIR := /lib/modules/$(shell uname -r)/build
#KDIR := /usr/src/linux-3.1.4/
#KDIR := /lib/modules/3.0.0-12-generic/build
PWD := $(shell pwd)

wrapfs-y := functions.o dentry.o file.o inode.o main.o super.o lookup.o mmap.o 

default:	
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules	
remov:
	sudo rmmod wrapfs
insert: default
	sudo insmod ./wrapfs.ko
reinsert: remov insert

clean:
	rm *.o
	rm *.ko
