//      restorepoint.c
//      
//      Copyright 2012 gaurav
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

int create_restore_point(char *rpname)
{
	int err = 0,cols;
	sqlite3 *handle;
	char *query;
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
		printf("\nquery = %s",query);
		fprintf(flog,"\nquery = %s",query);
		perror("\n Error creating table");
		goto error;
	}
	
	update_time=time(NULL);
	//printf("\ncurrent time %ld",update_time);
	
	sprintf(query,"INSERT INTO %s VALUES(%ld,'%s')",RP_TABLE,update_time,rpname);
	//printf("\nquery=%s",query);
	
	err = sqlite3_exec(handle,query,0,0,0);
	
	if(err)
	{
		printf("\nGot error while inserting data into table -> %s",RP_TABLE);
		fprintf(flog,"\nGot error while inserting data into table -> %s",RP_TABLE);
		printf("\nquery = %s",query);
		fprintf(flog,"\nquery = %s",query);
		perror("\n Error inserting into table");
		goto error;
	}
	
	
	error:
	free(query);
	sqlite3_close(handle);
	
	return update_time;	
}

time_t get_rp_time(char *rpname)
{
	int err = 0,cols;
	sqlite3 *handle;
	char *query;
	const char *val;
	sqlite3_stmt *stmt;

	
	query = (char*) malloc(4*SZ);
	err=get_db_handle("/",&handle);
	
	if(err)
	{
		printf("\nGot error while trying handle for /");
		perror("\n Error getting db handle");
		err = -1;
		goto error;
	}
	
	sprintf(query,"select rp_time from %s where rp_name='%s';",RP_TABLE,rpname);
	
	err = sqlite3_prepare_v2(handle,query,-1,&stmt,0);
	if(err)
	{
		printf("Selecting data from DB Failed in restorepoint\n");
		perror("Error");
		err = -1;
		goto error;
	}
	
	// Read the number of rows fetched
	cols = sqlite3_column_count(stmt);
	printf("\nrpcols=%d",cols);
	
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
		
		if(val == NULL)
		{
			sqlite3_finalize(stmt);
			sqlite3_close(handle);
			err = -1;
			goto error;
		}
		err = atoi(val);
	}
	else
	{
		err = -1;
		goto error;
	}
	
	error:
	sqlite3_finalize(stmt);
	sqlite3_close(handle);
	free(query);
	return err;	
}


int delete_restore_point(char *mountpoint,time_t update_time)
{
	time_t next_update_time;
	int err = 0,cols, col;
	sqlite3 *handle;
	char *query, *old_path;
	const char *val;
	sqlite3_stmt *stmt;
	
	query = (char*) malloc(4*SZ);
	old_path=(char*) malloc(SZ);
	
	err=get_db_handle(mountpoint,&handle);
	
	if(err)
	{
		printf("\nGot error while trying handle for /");
		perror("\n Error getting db handle");		
		goto error;
	}
	
	snprintf(query,4*SZ,"select max(rp_time) from (select rp_time from %s where rp_time<%ld);",RP_TABLE,update_time);
	
	printf("\nquery = %s",query);
	err = sqlite3_prepare_v2(handle,query,-1,&stmt,0);
	if(err)
	{
		printf("Selecting data from DB Failed in restorepoint\n");
		perror("Error");		
		goto error;
	}
	
	// Read the number of rows fetched
	cols = sqlite3_column_count(stmt);
	printf("\nrpcols=%d",cols);
	
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
		
		if(val == NULL)
		{
			sqlite3_finalize(stmt);
			sqlite3_close(handle);
			next_update_time = 0;
		}
		else
		{
			next_update_time = atoi(val);
		}
	}
	else
	{
		next_update_time = -1;
		goto error;
	}
	
	
	snprintf(query,4*SZ,"select * from %s where rp_time=%ld and access_time>%ld ;",BACKUP_TABLE,update_time,next_update_time);
	
	printf("\nquery = %s",query);
	err = sqlite3_prepare_v2(handle,query,-1,&stmt,0);
	if(err)
	{
		printf("Selecting data from DB Failed in restorepoint\n");
		perror("Error");		
		goto error;
	}
	
	// Read the number of rows fetched
	cols = sqlite3_column_count(stmt);
	printf("\nrpcols=%d",cols);
	
	while(1)
	{
		// fetch a row’s status
		err = sqlite3_step(stmt);	

		if(err == SQLITE_ROW)
		{	
			printf("\n");
			for(col=0 ; col<cols;col++)
			{
				val = (const char*)sqlite3_column_text(stmt,col);
				printf("%d:%s = %s\t",col,sqlite3_column_name(stmt,col),val);
			}
			
			printf("\n");
			val=(const char*)sqlite3_column_text(stmt,3);
			get_old_path(old_path,mountpoint,val);
			
			printf("\nDeleting %s",old_path);
			err = unlink(old_path);
			if(err)
			{
				perror("Error unlinking file");
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
			goto error;
		}
	}
	
	sqlite3_finalize(stmt);
	
	snprintf(query,4*SZ,"delete from %s where rp_time=%ld and access_time>%ld ;",BACKUP_TABLE,update_time,next_update_time);	
	printf("\n%s",query);
	fprintf(flog,"\n%s",query);
	err = sqlite3_exec(handle,query,0,0,0);
	if(err)
	{
		printf("Error executing query");
		goto error;
	}
	
	snprintf(query,4*SZ,"update %s set rp_time=%ld where rp_time=%ld;",BACKUP_TABLE,next_update_time,update_time);
	printf("\n%s",query);
	fprintf(flog,"\n%s",query);
	err = sqlite3_exec(handle,query,0,0,0);
	if(err)
	{
		printf("Error executing query");
		goto error;
	}
	
	snprintf(query,4*SZ,"delete from %s where rp_time=%ld;",RP_TABLE,update_time);
	printf("\n%s",query);
	fprintf(flog,"\n%s",query);
	err = sqlite3_exec(handle,query,0,0,0);
	if(err)
	{
		printf("Error executing query");
		goto error;
	}
	
	error:
	sqlite3_finalize(stmt);
	sqlite3_close(handle);
	free(query);
	free(old_path);
	
	return err;
	
}


int delete_rp(char *rpname)
{	
	time_t update_time;
	
	update_time = get_rp_time(rpname);
	if(update_time==-1)
	{
		printf("\nNo such restore point exists!");
		return -1;
	}
	
	do_each_mnpt(rpname,update_time,delete_restore_point);
	
}