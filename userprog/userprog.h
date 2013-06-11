//      user_prog.h
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

# include <sqlite3.h>
# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <string.h>
# include <mntent.h>
# include <assert.h>
# include <time.h>
# include <sys/time.h>
# include <sys/ioctl.h>


# include <errno.h>


# define SAVE_DIR "sls-data"
# define BACKUP_TABLE "backup"
# define RP_TABLE "restorepoint"
# define DEVICE_FILE "/dev/chardev"
//# define DEVICE_FILE "/outtt"
# define MOUNT_FILE "/etc/mtab"


# define MODE 4
# define UID 5
# define GID 6
# define ACCESSTIME 7


# define SZ 4096
# define PARTITION_COUNT 10
# define MAX_UPDATES SZ/4


/* Ioctl constants */
# define UPDATE_MODIF_TYM 1

extern FILE *flog;
extern int running;

int backup();
int do_each_mnpt();
int create_restore_point(char *rpname);

/* helper functions */
int allocate_2D(char *array[PARTITION_COUNT],int num,int size);
void free_2D(char *array[PARTITION_COUNT],int num);
inline int get_old_path(char *old_path,char *mount_point_path,const char *renamed_path);
int do_each_mnpt(char *rp_name, time_t update_time , int(*function)(char *, time_t));

/* database functions */
int open_db(char* dbname ,sqlite3 **handle);
int get_db_handle(char *mountpoint_path,sqlite3 **handle);
int create_backup_table(char* table_name,sqlite3 *handle);
int insert_into_table(sqlite3 *handle,long int rp_time,char* table_name,char* update_type,char* old_name,char* new_name,int perm,int uid,int gid,long long a_time);
int delete_table(char* table_name,sqlite3 **handle);


