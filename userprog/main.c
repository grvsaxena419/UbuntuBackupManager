//      main.c
//      
//      Copyright 2012 gaurav <gaurav@gs-laptop>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
//      
//      

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>

# include "userprog.h"

int running=1;
FILE *flog;

int main(int argc, char **argv)
{	
	// start backup in a separate process if asked for it
	void *stack_space;
	int err;
	int log_fd;
	int uid;
	
	uid = getuid();
	
	if(uid!=0)
	{
		printf("\nYou need to be root to execute this application\n");
		return -1;
	}
	
	
/*
	log_fd = open("backupfs.log",O_WRONLY|O_TRUNC|O_DIRECT|O_SYNC,0);
	if(!log_fd)
	{
		perror("Unable to open log file");
		return -1;
	}
*/
	flog=fopen("backupfs.log","w");
	if(!flog)
	{
		perror("Unable to open log file");
		// exit;
		err = errno;
	}
	
	if(argc<2)
	{
		printf("%s <option> <rpname>",argv[0]);
		return -1;
	}
	if(strcmp(argv[1],"backup")==0)
	{
/*
		printf("\nstarting process");
		fflush(stdout);
		stack_space=(void*) malloc(65536);
		printf("\nstack process");
		fflush(stdout);
		err = clone(backup, stack_space+65536, CLONE_VM|CLONE_FILES, NULL);
		if(err<0)
		{
			perror("\nerror while cloning");
		}
		printf("\ncloned process started");
		fflush(stdout);
*/
		backup();
		while(1)
		{
			// accept input and take action
			sleep(1);
		}
	}
	else if(strcmp(argv[1],"restore")==0)
	{
		printf("\nrestore rp named %s",argv[2]);
		if(argc<3)
		{
			printf("%s <restore> <rpname>\n",argv[0]);
		}
		else
		{
			restore(argv[2]);
		}
		
	}
	else if(strcmp(argv[1],"create_rp")==0)
	{
		printf("\nCreate rp named %s",argv[2]);
		if(argc<3)
		{
			printf("%s <create_rp> <rpname>\n",argv[0]);
		}
		else
		{
			create_restore_point(argv[2]);
		}
	}
	else if(strcmp(argv[1],"delete_rp")==0)
	{
		printf("\ndelete rp named %s",argv[2]);
		if(argc<3)
		{
			printf("%s <delete_rp> <rpname>\n",argv[0]);
		}
		else
		{
			delete_rp(argv[2]);
		}
	}
	else
	{
		printf("\n------------------\n");
	}
}
