//      restore.c
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


#include "userprog.h"

int parse_partition_types(FILE *fin,char *partition_type_list[PARTITION_COUNT],int size)
{
	int i,err;
	i=0;
	char buffer[SZ];
	char temp[SZ];
	
	while(1)
	{
		err=fscanf(fin,"%[^\n]",buffer);
		fgetc(fin);
		if(err<0)
		{
			perror("fscanf");
			break;
		}
		else if(err == 0)
		{
			break;
		}
		//printf("buffer=%s",buffer);

		/* this is quite static try something better ! */
		sscanf(buffer,"\n%s %s",temp,partition_type_list[i]);
		if(strcmp(temp,"nodev")==0)
			continue;
		else 
		{
			//printf("temp=%s partition_type_list=%s",temp,partition_type_list[i]);
			strncpy(partition_type_list[i],temp,SZ);
			i++;
		}
		
		if(i>=size)
			break;
	}
	return i;
}



int read_mount_list(char *partition_list[PARTITION_COUNT],int size,char *partition_types[PARTITION_COUNT],int count)
{
	FILE *mount_file;
	struct mntent* mount_st;
	int i,ctr;
	
	mount_file = setmntent (MOUNT_FILE, "r");
	if(!mount_file)
	{
		perror("unable to open mount_file");
		return -1;
	}
	
	ctr=0;
	errno = 0;
	while(1)
	{
		mount_st = getmntent (mount_file);
		if(!mount_st)
		{
			if(errno != 0)
				perror("\nerror in getmntent");
			break;
		}
		//printf("\nopts=%s",mount_st->mnt_opts);
		for(i=0;i<count;i++)
		{
			if(strcmp(partition_types[i],mount_st->mnt_type)==0)
			{
				printf("\nFound a partition type=%s mount_point=%s",mount_st->mnt_type,mount_st->mnt_dir);
				strncpy(partition_list[ctr],mount_st->mnt_dir,SZ);
				ctr++;
				break;
			}
		}
		
	}
	return ctr;
}



int check_for_exist(char *new_path)
{
	int err = 0;
	char *renamed_exist;
	struct stat buf;
		
	err = stat(new_path,&buf);
	if(err!=ENOENT)
	{
		// file already exists ! rename it to .timestamp so its unique
		renamed_exist = (char*) malloc(SZ+10);
		sprintf(renamed_exist,"%s.%d",new_path,(int)time(NULL));
				
		err = rename(new_path,renamed_exist);
		free(renamed_exist);
	}
	
	return err;	
}

int mkdir_parent(char *path, mode_t mode)
{
    struct stat st;
    int status = 0;

    if (stat(path, &st) != 0)
    {
        /* Directory does not exist */
        if (mkdir(path, mode) != 0)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }

    return(status);
}


/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
int make_path(char *path, mode_t mode)
{
    char *pp, *sp ,*copypath;
    int status;
    copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = mkdir_parent(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
/*
    if (status == 0)
        status = mkdir_parent(path, mode);
*/
    free(copypath);
    return (status);
}


int create_copy(char *old_path,char *new_path)
{
	int err;
	char *command;
	command = (char*) malloc(4*SZ);
	snprintf(command,4*SZ,"cp \"%s\" \"%s\"",old_path,new_path);
	err = system(command);
	
	free(command);
	return err;
}


int restore_backup(char *mount_point, time_t update_time)
{
	char *query;
	int err , col, cols;
	char *old_path,*new_path;
	const char *val;
	sqlite3 *handle;
	// A statement for fetching tables
	sqlite3_stmt *stmt;
		
	struct timespec times_path[2];
	int mode,uid,gid;
	
	query = (char*) malloc(4*SZ);
	old_path=(char*) malloc(SZ);
	new_path=(char*) malloc(SZ);
	
	err = get_db_handle(mount_point,&handle);
		
	if(err)
	{
		printf("\nGot error while trying handle for %s",mount_point);
		perror("\n Error getting db handle");
		goto error;
	}
	
	snprintf(query,4*SZ,"select * from %s where access_time<=%ld and rp_time>=%ld order by update_time_sec desc,update_time_usec desc;",BACKUP_TABLE,update_time,update_time);
	fprintf(flog,"%s",query);
	//err = sqlite3_exec(handle,query,0,0,0);
	
	err = sqlite3_prepare_v2(handle,query,-1,&stmt,0);
	if(err)
	{
		printf("Selecting data from DB Failed\n");
		printf("\nquery = %s",query);
		perror("error");
		goto error;
	}
	
	// Read the number of rows fetched
	cols = sqlite3_column_count(stmt);
	
    while(1)
	{
		// fetch a row’s status
		err = sqlite3_step(stmt);

		if(err == SQLITE_ROW)
		{
			// SQLITE_ROW means fetched a row

			// sqlite3_column_text returns a const void* , typecast it to const char*
			for(col=0 ; col<cols;col++)
			{
				val = (const char*)sqlite3_column_text(stmt,col);
				printf("%s = %s\t",sqlite3_column_name(stmt,col),val);
			}
			printf("\n");
			
			/*0:rp_time = 1335515673	1:update_type = MODFILE	2:old_path = /abc/a	3:new_path = c40b3befe149f5b015bd5348be828aee	
			 * 4:permission = 33188 	5:uid = 0	6:gid = 0	7:access_time = 1335515585	8:update_time_sec = 1335515745	
			 * 9:update_time_usec = 783794*/
			val=(const char*)sqlite3_column_text(stmt,2);
			strcpy(new_path,val);
						
			val=(const char*)sqlite3_column_text(stmt,1);
			if(strcmp(val,"RENAME")==0)
			{
				check_for_exist(new_path);
				val=(const char*)sqlite3_column_text(stmt,3);
				get_old_path(old_path,mount_point,val);
				
				// handle rename by reversing it
				printf("\nrenaming file %s to %s",old_path,new_path);
				make_path(new_path,0);
				err=rename(old_path,new_path);
				if(err)
				{
					perror("Error in RENAME rename");
				}
			}
			else if(strcmp(val,"RMDIR")==0)
			{
				//val=(const char*)sqlite3_column_text(stmt,2);
				//strcpy(new_path,val);
				
				//check_for_exist(new_path);
				make_path(new_path,0);
				err = mkdir(new_path,0);
				printf("\ncreating directory named %s",new_path);
				// check for errors 
				if(err)
				{
					perror("Error in mkdir");
				}
			}
			else if(strcmp(val,"UNLINK")==0)
			{
				//val=(const char*)sqlite3_column_text(stmt,2);
				//strcpy(new_path,val);
				
				//check_for_exist(new_path);
				check_for_exist(new_path);
				val=(const char*)sqlite3_column_text(stmt,3);
				get_old_path(old_path,mount_point,val);
				
				printf("\nrenaming file %s to %s",old_path,new_path);	
				
				make_path(new_path,0);
				//err=rename(old_path,new_path);
				err = create_copy(old_path,new_path);
				// check for errors
				if(err)
				{
					perror("Error in UNLINK rename");
				}
			}
			else if(strcmp(val,"MODFILE")==0)
			{
				//val=(const char*)sqlite3_column_text(stmt,2);
				//strcpy(new_path,val);
				
				//check_for_exist(new_path);
				
				val=(const char*)sqlite3_column_text(stmt,3);
				get_old_path(old_path,mount_point,val);
				
				printf("\nrenaming file %s to %s",old_path,new_path);	
				
				make_path(new_path,0);
				//err=rename(old_path,new_path);
				err = create_copy(old_path,new_path);
				// check for errors 
				if(err)
				{
					perror("Error in MODFILE rename");
				}
			}
			
			// restoring permissions ownership and time
			val=(const char*)sqlite3_column_text(stmt,MODE);
			err = chmod(new_path,atoi(val));
			if(err!=0)
			{
				printf("\n%s",new_path);
				perror("\nError in changing file permissions");
				//return -1; // temporary error for a file doesn't mean a big setback
			}
			
			val=(const char*)sqlite3_column_text(stmt,UID);
			uid=atoi(val);
			val=(const char*)sqlite3_column_text(stmt,GID);
			gid=atoi(val);
			
			err = chown(new_path,uid,gid);
			
			if(err != 0)
			{
				printf("\n%s",new_path);
				perror("Error in changing file ownership");
				//return -1; // temporary error for a file doesn't mean a big setback
			}
			
			val=(const char*)sqlite3_column_text(stmt,ACCESSTIME);				
			times_path[0].tv_sec=times_path[1].tv_sec=atoll(val);
			times_path[0].tv_nsec=times_path[1].tv_nsec=0;
			err = utimensat(0,new_path,times_path,0);				
			if(err != 0)
			{
				printf("\n%s",new_path);
				perror("Error in changing file access time");
				//return -1; // temporary error for a file doesn't mean a big setback
			}
			
		}
		else if(err == SQLITE_DONE)
		{
			// All rows finished
			printf("All rows fetched\n");
			break;
		}
		else
		{
			// Some error encountered
			printf("Some error encountered\n");
			break;
		}
	}
	
	error:
	free(query);
	free(old_path);
	free(new_path);
	
	return err;	
}


int restore(char *rp_name)
{
	time_t update_time;
	
	update_time = get_rp_time(rp_name);
	if(update_time==-1)
	{
		printf("\nNo such restore point exists!");
		return -1;
	}
	printf("\n update_time=%ld",update_time);
	
	do_each_mnpt(rp_name,update_time,restore_backup);
	
}


