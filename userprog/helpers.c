//      helpers.c
//      
//      Copyright 2012 gs <gs@gs-laptop>
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

# include "userprog.h"


int allocate_2D(char *array[PARTITION_COUNT],int num,int size)
{
	int i;
	for(i=0;i<num;i++)
	{
		array[i]=(char*)malloc(size);
	}
	return 0;
}

void free_2D(char *array[PARTITION_COUNT],int num)
{
	int i;
	for(i=0;i<num;i++)
	{
		free(array[i]);
	}
	
}

inline int get_old_path(char *old_path,char *mount_point_path,const char *renamed_path)
{
	sprintf(old_path,"%s/%s/%s",mount_point_path,SAVE_DIR,renamed_path);
}

int do_each_mnpt(char *rp_name, time_t update_time , int(*function)(char *, time_t))
{
	// Assumes that total restore is needed restore everything in different folders 
	
	char mount_info_file[]="/proc/mounts";
	char filesystems_info_file[]="/proc/filesystems";
	
	char *partition_types[PARTITION_COUNT],*partition_list[PARTITION_COUNT];
	char *mount_dir;
	int i,count, mounts_count, err;
	FILE *mount_file,*partition_file;
	struct stat st;
	
	mount_file=fopen(mount_info_file,"r");
	if(!mount_file)
	{
		perror("unable to open mount_file");
		//exit
	}
	
	partition_file=fopen(filesystems_info_file,"r");
	if(!partition_file)
	{
		perror("unable to open partition_file");
		//exit
	}
	
	allocate_2D(partition_types,PARTITION_COUNT,SZ);
	
	count = parse_partition_types(partition_file,partition_types,PARTITION_COUNT);
	
/*
	printf("\ncount=%d",count);
	for(i=0;i<count;i++)
	{
		printf("\n%s",partition_types[i]);
	}
*/
	
	allocate_2D(partition_list,PARTITION_COUNT,SZ);
	
	mounts_count = read_mount_list(partition_list,PARTITION_COUNT,partition_types,count);
	
	mount_dir = (char*)malloc(SZ);
	

	printf("\nmounts_count=%d",mounts_count);
	for(i=0;i<mounts_count;i++)
	{
		printf("\nmount_point=%s",partition_list[i]);
		printf("\nTrying to restore backup from this mountpoint");
		snprintf(mount_dir,SZ,"%s/%s",partition_list[i],SAVE_DIR);
		err = stat(mount_dir,&st);
		if(err)
		{
			perror("Error");
		}
		else
		{
			function(partition_list[i],update_time);
		}
		
	}	
	
	free_2D(partition_types,PARTITION_COUNT);
	free_2D(partition_list,PARTITION_COUNT);
	free(mount_dir);
	sleep(10);
	return 0;
}

