//      sqlite_db_functions.c
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

int open_db(char* dbname ,sqlite3 **handle)
{
	int err;
	
	err = sqlite3_open(dbname,handle);
	return err;
}

int get_db_handle(char *mountpoint_path,sqlite3 **handle)
{
	char *db_path;
	int err;
	
	db_path=(char*)malloc(SZ);
	strcpy(db_path,mountpoint_path);
	strcat(db_path,"/");
	strcat(db_path,SAVE_DIR);
	err = mkdir(db_path,0);
	
	strcat(db_path,"/backupfs.db");
	fprintf(flog,"\ntrying to open %s",db_path);
	printf("\ntrying to open %s",db_path);
	
	err = open_db(db_path,handle);
	free(db_path);
	return err;	
}



int create_backup_table(char* table_name,sqlite3 *handle)
{
	int err;
	char *query;
	query=malloc(4*4096);
	
	sprintf(query,"CREATE TABLE IF NOT EXISTS %s (rp_time INTEGER NOT NULL,update_type TEXT NOT NULL,old_path TEXT NOT NULL,new_path TEXT NOT NULL,permission INTEGER,uid INTEGER,gid INTEGER,access_time INTEGER,update_time_sec INTEGER,update_time_usec INTEGER , PRIMARY KEY(update_time_sec,update_time_usec));", table_name);
    
    err= sqlite3_exec(handle,query,0,0,0);
    fprintf(flog,"\n%s",query);
    free(query);
    return err;
}

int insert_into_table(sqlite3 *handle,long int rp_time,char* table_name,char* update_type,char* old_name,char* new_name,int perm,int uid,int gid,long long a_time)
{
	int err;
	char *query;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	
	query=malloc(4*4096);	
	
	sprintf(query,"INSERT INTO %s VALUES(%ld,'%s','%s','%s','%d','%d','%d','%lld',%ld,%ld);",table_name,rp_time,update_type,old_name,new_name,perm,uid,gid,a_time,tv.tv_sec,tv.tv_usec);
	
	err = sqlite3_exec(handle,query,0,0,0);
	fprintf(flog,"\n%s",query);
	free(query);
    return err;
}





int delete_table(char* table_name,sqlite3 **handle)
{
	char table[100];
	sprintf(table,"drop table if exists %s",table_name);
	return (sqlite3_exec(*handle,table,0,0,0));
}

int close_db(sqlite3 *handle)
{
	return(sqlite3_close(handle));
}

//int main()
//{
	//int result;
	//sqlite3 *handle;
	//result=open_db("sls",&handle);
	//printf("\nresult = %d\n",result);
	//result=create_table("backup1",&handle);
	//printf("\nresult = %d\n",result);
	//result=insert_into_table("backup1",&handle,"del","abc","def",111,0,88,1234567);
	//printf("\nresult = %d\n",result);
	//result=delete_table("backup1",&handle);
	//result=close_db(&handle);
	//printf("\nresult = %d\n",result);
	//return 0;
//}
