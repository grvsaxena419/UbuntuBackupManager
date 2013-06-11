//      backup.c
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

# include "userprog.h"


int parse_string(char *buff,char parsed_buff[MAX_UPDATES][SZ])
{
	char *delim="\n",*str,*token,*saveptr1;
	int j,i=0;
	for (j = 1,str=buff; ; j++, str = NULL) 
	{
		token = strtok_r(str, delim, &saveptr1);
		if (token == NULL)
           break;
		
		strcpy(parsed_buff[i],token);
        printf("%d: %s\n", i, parsed_buff[i]);
        fprintf(flog,"%d: %s\n", i, parsed_buff[i]);
        i++;
               /* for (str2 = token; ; str2 = NULL) {
                   subtoken = strtok_r(str2,a3, &saveptr2);
                   if (subtoken == NULL)
                     break;
                   printf(" --> %s\n", subtoken);
               }*/
	}
	return i;
	
}

int parse_event_buffer(char *buff, int buflen, int *len, char parsed_buff[MAX_UPDATES][SZ])
{
	char *c,*last_bracket;
	int i ,j ,counter ,event_count , last_open, last_close;
	i = 0;
	event_count = 0;
	
	last_bracket =(char*) memrchr(buff,'}',buflen);
	
	assert(last_bracket!=NULL);
			
	while(i<buflen&&i<last_bracket-buff)
	{
		if(buff[i]=='{')
		{
			last_open = i;
			i++;
			
			while(buff[i]!='}'&&i<last_bracket-buff)
			{	
				j = 0;

				while(buff[i]!='\n'&&buff[i]!='}')
				{
					parsed_buff[event_count][j] = buff[i];
					i++;
					j++;
				}
				
				parsed_buff[event_count][j]=0;
				event_count++;

				if(buff[i]=='}')
				{
					last_close = i;
					i++;
					break;
				}
				else if(buff[i]=='\n')
				{
					i++;
				}
			}
		}
		else
		{
			break;
		}
	}
	

	//if(last_close<last_open)
	if(buff[i]) // buff has not been completely parsed
	{
		for(c=last_bracket+1,j=0;*c&&i<buflen;c++,i++,j++)
		{
			buff[j]=*c;		// copy the buffer contents to start
		}
		*len = j;
		buff[j]=0;
	}
	else
	{
		*len = 0;
	}
		
	return event_count;
}


int setup_restore_points()
{
	int err = 0,cols ,col;
	sqlite3 *handle;
	char *query;
	const char *val;
	sqlite3_stmt *stmt;
	time_t update_time;
	
	query = (char*) malloc(4*SZ);
	err=get_db_handle("/",&handle);
	
	if(err)
	{
		printf("\nGot error while trying handle for /");
		perror("\n Error getting db handle");
		goto error;
	}
	

	sprintf(query,"CREATE TABLE IF NOT EXISTS %s (rp_time INTEGER PRIMARY KEY,rp_name TEXT UNIQUE NOT NULL)", RP_TABLE);
	err = sqlite3_exec(handle,query,0,0,0);
	if(err)
	{
		printf("\nGot error while creating table -> %s",RP_TABLE);
		fprintf(flog,"\nGot error while creating table -> %s",RP_TABLE);
		fprintf(flog,"\n query = %s",query);
		perror("\n Error creating table");
		goto error;
	}

	
	sprintf(query,"select max(rp_time) from %s;",RP_TABLE);
	
	err = sqlite3_prepare_v2(handle,query,-1,&stmt,0);
	if(err)
	{
		printf("Selecting data from DB Failed\n");
		fprintf(flog,"Selecting data from DB Failed\n");
		fprintf(flog,"\n query = %s",query);
		perror("Error");
		goto error;
	}
	
	// Read the number of rows fetched
	cols = sqlite3_column_count(stmt);
	printf("\ncols=%d",cols);
	fprintf(flog,"\ncols=%d",cols);
	
	err = sqlite3_step(stmt);

	if(err == SQLITE_ROW)
	{	
/*
		for(col=0 ; col<cols;col++)
		{
			val = (const char*)sqlite3_column_text(stmt,col);
			printf("%d:%s = %s\t",col,sqlite3_column_name(stmt,col),val);
		}
*/
		val=(const char*)sqlite3_column_text(stmt,0);
		printf("\nval=%s",val);
		fprintf(flog,"\nval=%s",val);
		
		if(val == NULL)
		{
			sqlite3_finalize(stmt);
			sqlite3_close(handle);
			printf("\nNo restore point found!");
			printf("\nCreating a new restore point");
			err = create_restore_point("First");
			if(!err)
			{
				
			}
			goto no_error;
		}
		err = atoi(val);
	}
	else
	{
		goto error;
	}
	

	error:	
	sqlite3_finalize(stmt);
	sqlite3_close(handle);
	no_error:
	free(query);
	return err;
	
}


int backup()
{
	int err = 0,fd,i,j,count=0, num_of_records,remain_records;
	char buff[SZ];
	int rslt;
	long int latest_modif_tym=0;
	struct timespec t1,t11[2];
	char parsed_buff[MAX_UPDATES][SZ];
	int ctr=0;
	sqlite3 *handle;
	
	
	if ((fd=open(DEVICE_FILE, O_RDONLY))<0)
	{
		perror("\nError in opening device file *SEVERE* error");
		return -1;
	}
	
/*
	printf("Backup....");
	fflush(stdout);
*/
	
	// setup last restore point time in device
	err = setup_restore_points();
	if(err<0)
	{
		printf("setup_restore_point complete err = %d",err);
		fflush(stdout);
		perror("Error in setup_restore_points");
	}
	else
	{
		latest_modif_tym = err;
	}
	err = ioctl(fd,UPDATE_MODIF_TYM,latest_modif_tym);
	
	if(err)
	{
		printf("\n error in ioctl err = %d",err);
		fprintf(flog,"\n error in ioctl err = %d",err);		
		perror("Received error ");
	}
	
	memset(buff, 0, SZ);
	
	// do this intelligently in a separate thread 
	while(running)
	{
		printf("\n Old count = %d",count);
		fprintf(flog,"\n Old count = %d",count);
		
		rslt=read(fd, buff + count, SZ - count);
		
		if(rslt<0)
		{
			perror("\nError in reading device file *SEVERE* error");
			return -1;
		}
		
		printf("\nread result=%d count = %d\n\n",rslt,count);
		fprintf(flog,"\nread result=%d count = %d\n\n",rslt,count);
		if(rslt==0)
		{
/*
			if(count!=0)
			{
				rslt = count;
			}
			else
*/
			{
				perror("\nRead result = 0 \nThis is a *SEVERE* Error PLease verify if the chardev is working");
				return -1;
			}
		}
		
		
		for(i=0;i<rslt;i++)
		{
			if(i==count)
				printf("\n new buff starts\n");
			printf("%c",buff[i]);
		}
//		count = 0;
		
		printf("\n\n");
		//rslt=parse_string(buff,parsed_buff);
		rslt = parse_event_buffer(buff,rslt+count,&count,parsed_buff);
		printf("\nrslt=%d count = %d",rslt,count);
		fprintf(flog,"\nrslt=%d count = %d",rslt,count);
		
		for(i=0;i<rslt;i++)
		{
			printf("\n%d:%s",i,parsed_buff[i]);
			fprintf(flog,"\n%d:%s",i,parsed_buff[i]);
		}
		
		
		if((rslt)%8!=0)
		{
			printf("\nError rslt is not a multiple of 8\nNeed to correct this parsing error");
/*
			num_of_records=(rslt)/8;			
			remain_records=rslt-num_of_records*8;
			printf("\nnum_of_recordes=%d remain_records=%d",num_of_records,remain_records);
			for(i=num_of_records;i<rslt;i++)
			{
				for(j=0;parsed_buff[i][j];j++)
				{
					buff[count]=parsed_buff[i][j];
					count++;
				}
				if(i<rslt-1)
				{
					buff[count]='\n';
					count++;
				}
			}
			rslt=num_of_records*8;
*/
		}
		
		for(i=0;i<(rslt)/8;i++)
		{
			err=get_db_handle(parsed_buff[i*8+1],&handle);
			if(err)
			{
				printf("\nGot error while trying handle for %s",parsed_buff[i*8+1]);
				perror("\n Error getting db handle");
				continue;
			}

			err = create_backup_table(BACKUP_TABLE,handle);
			if(err)
			{
				printf("\nGot error while creating table -> %s",BACKUP_TABLE);
				perror("\n Error creating table");
				continue;
			}

			err = insert_into_table(handle,latest_modif_tym,BACKUP_TABLE,parsed_buff[i*8+0],parsed_buff[i*8+2],parsed_buff[i*8+3],atoi(parsed_buff[i*8+4]),atoi(parsed_buff[i*8+5]),atoi(parsed_buff[i*8+6]),atoll(parsed_buff[i*8+7]));
			if(err)
			{
				printf("\nGot error while inserting into table ");
				perror("\n Error inserting into table");
				continue;
			}
			err = close_db(handle);
			fflush(flog);
		}
	}
		
	
	return err;
}
