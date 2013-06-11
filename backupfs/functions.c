/*
 * Author gs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

# include "wrapfs.h"


//int modif_tym=1330933291u;
__kernel_time_t modif_tym=0u;

struct timer_list update_timer;

/* 
 * This function finds a valid path to rename the deleted file
 * after getting path we just rename the file instead of deleting it
 */
int getrename_path(struct path lowerpath, struct path *finalpath, char * mountpoint_path)
{		
	struct dentry *victim_dentry; // to be deleted 
	struct dentry *mountedon;   // dentry of mountpoint		
	int buflen = PAGE_SIZE,len, err=0;
	char *buf1,*buf2,*newname,*retval,*complete_path;
	char backup_directory[]=SAVEDIR;		
	struct nameidata lower_nd,lower_finalnd;	
	
	
	victim_dentry=lowerpath.dentry;
	printk(KERN_EMERG "Wrapfs: getrename path > Got request to delete %s",victim_dentry->d_name.name);
	
	newname=(char*)kzalloc(buflen, GFP_KERNEL);
	buf1=(char*)kzalloc(buflen, GFP_KERNEL);	
	buf2=(char*)kzalloc(buflen, GFP_KERNEL);
	complete_path=(char*)kzalloc(buflen, GFP_KERNEL);
	
	
	// only for printing name not actually needed
	retval=dentry_path_raw(victim_dentry,buf1,buflen);
	
	if(IS_ERR(retval))
	{
		printk(KERN_EMERG "Wrapfs:serched path error");
		goto error_name;
	}
	else
	{
		printk(KERN_EMERG "Wrapfs:serched path = %s ",retval);
	}
	
	snprintf(complete_path,buflen,"%s.%ld",retval,modif_tym);
	/* a more complex name creation procedure needed*/
	err = get_hashed_path(complete_path,newname,buflen);
	if(err<0)
	{
		printk(KERN_EMERG" errrrrrrrrrrrrrrrror in hashing ");
		goto error_name;
	}

	len=strlen(newname);
	printk(KERN_EMERG"newname=%s",newname);
	
		
	// the dentry of mountpoint
	mountedon=victim_dentry->d_sb->s_root;

	printk(KERN_EMERG "Wrapfs: mntd on getrename path > %s d_inode=%p operations=%p  test opes=%p is root=%d inode_num=%ld",mountedon->d_name.name,mountedon->d_inode,mountedon->d_op,mountedon->d_fsdata,IS_ROOT(mountedon),mountedon->d_inode->i_ino);			

	// only for debug purposes
	retval=dentry_path_raw(lowerpath.mnt->mnt_mountpoint,buf1,buflen);
	if(!IS_ERR(retval))
	{
		printk(KERN_EMERG "Wrapfs:root path = %s",retval);	
		strncpy(mountpoint_path,retval,buflen);
	}
	else
	{
		printk(KERN_EMERG "Error in finding path of mountpoint");
		goto error_name;
	}

	printk(KERN_EMERG "calling frst vfs_lookup");			

	// lookup for sls-data in mountpoint / or any other
	err = vfs_path_lookup(mountedon, lowerpath.mnt, backup_directory, 0,&lower_nd);

	printk(KERN_EMERG "called lookups err=%d",err);	
	if(err)
	{
		printk(KERN_EMERG "Wrapfs: %s lookup failed err=%d",backup_directory,err);
		if(err==-ENOENT)
		{
			// no directory sls-data found Try creating one
			printk(KERN_EMERG"\n No backup directory found\n Creating directory");
			err=vfs_mkdir(mountedon->d_inode,lower_nd.path.dentry,0);
			if(!err)
			{
				// directory created no error
				printk(KERN_EMERG"\n Successfully Created directory");
				goto no_error;
			}
		}
		// return the error *severe*
		//return err;
		goto error_name;
	}
	else
	{
		//printk(KERN_EMERG"wrapfs: no error in movepath  path > %s d_inode=%d operations=%p",movepath->d_name.name,movepath->d_inode,movepath->d_op);
		printk(KERN_EMERG"wrapfs: no error in lookup of backup directory dentry =%p name=%s inode=%p op=%p",lower_nd.path.dentry,lower_nd.path.dentry->d_name.name,lower_nd.path.dentry->d_inode,lower_nd.path.dentry->d_op);
	}
	//pathcpy(&sls_dentry,&lower_nd.path);
	//validpath=1;
	
	no_error:
	
	//printk(KERN_EMERG"newname=%s",newname);
	//printk(KERN_EMERG"Calling secnd lookup");
	// lookup for the final file in sls-data
	err=vfs_path_lookup(lower_nd.path.dentry,lower_nd.path.mnt,newname,0,&lower_finalnd);
	//err=vfs_path_lookup(sls_dentry.dentry,sls_dentry.mnt,newname,0,&lower_finalnd);
	
	//printk(KERN_EMERG"Called secnd lookup");
	
	if(err)
	{
		if(err==-ENOENT)
		{
			// no error as file doesnt exist yet 
			printk(KERN_EMERG "Wrapfs: scnd lookup failed err=%d ",err);
			err=0;
		}
		else
		{
			// this is severe
			//return err;
			goto error_name;
		}
	}
	else
	{
		// not possible after cryptic unique name generation
		printk(KERN_EMERG"wrapfs: no error in krn_path so lowerparent path= means file already exists *OVERWRITE*");
	}

	retval=dentry_path_raw(lower_finalnd.path.dentry,buf1,buflen);
	
	if(IS_ERR(retval))
	{
		printk(KERN_EMERG "Wrapfs:serched path error");
		goto error_name;
	}
	else
	{
		printk(KERN_EMERG "Wrapfs: final path = %s ",retval);
	}
	
	// set the final path dentry
	pathcpy(finalpath,&lower_finalnd.path);
	
	error_name:
	kfree(buf1);
	kfree(buf2);
	kfree(newname);
	kfree(complete_path);
	
	printk(KERN_EMERG" Returning from getrnme path err = %d",err);
	
	return err;	
	
	
}


/* 
 * This method copies data from given path infile to another path outfile 
 * Copy is done by using cp and passing it absolute path of both files
 */
int copy_file(char* input_file,char* output_file)
{
	int err = 0;
	struct subprocess_info *si;
	
	char path[]="/bin/backupfs_copy";

	//char *argv[] = { "/bin/cp", "/root/gaurav", "/sls-data/gaurav.unrm", NULL };
	char *argv[4];
	
	char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", 
		NULL 
	};	
	
	//strcpy(argv[1],input_file);
	//strcpy(argv[2],output_file);
	//strcpy(argv[3],"\0");
	argv[0]=path;
	argv[1]=input_file;
	argv[2]=output_file;
	argv[3]=NULL;
	
	
	si=call_usermodehelper_setup(argv[0], argv,envp, GFP_KERNEL);
	
	if(IS_ERR(si))
	{
		printk(KERN_EMERG"wrapfs: Got error %ld trying to setup usrnodehelpr %s",PTR_ERR(si),argv[0]);
		err=PTR_ERR(si);
		goto error;
	}
	
	err=call_usermodehelper_exec( si, UMH_WAIT_PROC );
	
	if(err<0)
	{
		printk(KERN_EMERG"wrapfs: Got error %d while trying to execute the process ",err);
		//return err;
		goto error;
	}
		
	error:
	printk(KERN_EMERG"Returning from copy_file(%s,%s) with err=%d",input_file,output_file,err);
	return err;
}


/* 
 * This method backs up a file by passing it path of file to be backed up
 * it checks if file already backed up 
 */
int backup_file(struct path lower_path)
{
	int err = 0, buflen = PAGE_SIZE;
	char *buf,*buf1,*buf2,*buf3,*buf4,*mountpoint_path,*input_file,*output_file,*complete_path;
	char backup_directory[]=SAVEDIR;	
	
	buf=(char*)kzalloc(buflen, GFP_KERNEL);
	buf1=(char*)kzalloc(buflen, GFP_KERNEL);
	buf2=(char*)kzalloc(buflen, GFP_KERNEL);
	buf3=(char*)kzalloc(buflen, GFP_KERNEL);
	buf4=(char*)kzalloc(buflen, GFP_KERNEL);
	complete_path=(char*)kzalloc(buflen, GFP_KERNEL);
	
	
	input_file=dentry_path_raw(lower_path.dentry,buf1,buflen);
	if(IS_ERR(input_file))
	{
		printk(KERN_EMERG "wrapfs: input path error");
		err=PTR_ERR(input_file);
		goto error;
	}
	else
	{
		printk(KERN_EMERG "wrapfs: input file = %s",input_file);
	}
	
	//err=getrename_path(lower_path, &dest_path, mountpoint_path);
	
	printk(KERN_EMERG"mountpoint_path = %p mntpoint=%p name=%s",lower_path.mnt,lower_path.mnt->mnt_mountpoint,lower_path.mnt->mnt_mountpoint->d_name.name);
	mountpoint_path=dentry_path_raw(lower_path.mnt->mnt_mountpoint,buf3,buflen);
	printk(KERN_EMERG"\n mountpoint_path =%s",mountpoint_path);
	//return 0;
	
	if(err)
	{
		printk(KERN_EMERG"Error finding path");
		goto error;
	}

	//output_file=dentry_path_raw(dest_path.dentry,buf2,buflen);
	snprintf(complete_path,buflen,"%s.%ld",input_file,modif_tym);
	get_hashed_path(complete_path,buf4,buflen);


	//strcpy(buf2,mountpoint_path);
	//strcat(buf2,"/");
	//strcat(buf2,backup_directory);
	//strcat(buf2,"/");		
	//strcat(buf2,buf4);

	snprintf(buf2,buflen,"%s/%s/%s",mountpoint_path,backup_directory,buf4);
	


	strcpy(buf,mountpoint_path);
	//strcat(buf,"/");
	strcat(buf,input_file);



	printk(KERN_EMERG"Creating copy with name  i/p=%s o/p=%s",buf,buf2);
	//return 0;
	//printk(KERN_EMERG"here file attributes are fop=%p",file->f_op);
	output_file=buf2;
	err=copy_file(buf,output_file);
	if(err)
	{
		printk(KERN_EMERG"Error %d copying file",err);
		goto error;
	}
	
	/* MODFILE <sep> mountpoint_path <sep> Modifiedfile <sep> OldCopyfile <sep> mode(permission) <sep> uid <sep> gid <sep> accesstime_sec <sep> accesstime_nsec  */ 
	sprintf(buf,"{MODFILE\n%s\n%s\n%s\n%d\n%d\n%d\n%ld}",mountpoint_path,input_file,buf4,lower_path.dentry->d_inode->i_mode,lower_path.dentry->d_inode->i_uid,lower_path.dentry->d_inode->i_gid,(lower_path.dentry->d_inode->i_ctime.tv_sec));//+lower_dentry->d_inode->i_ctime.tv_nsec);
	char_dev_buffer_insert(buf,strlen(buf));
	//printk(KERN_EMERG"wrapfs:Inserted message%s",buf);
	
	error:
	kfree(buf);
	kfree(buf1);
	kfree(buf2);
	kfree(buf3);
	kfree(buf4);
	kfree(complete_path);
	
	printk(KERN_EMERG"Returing frm backup_file err=%d",err);
	//UDBG()
	return err;
}



/**
 * backupfs_to_hex
 * @dst: Buffer to take hex character representation of contents of
 *       src; must be at least of size (src_size * 2)
 * @src: Buffer to be converted to a hex string respresentation
 * @src_size: number of bytes to convert
 */
void backupfs_to_hex(char *dst, char *src, size_t src_size)
{
	int x;

	for (x = 0; x < src_size; x++)
		sprintf(&dst[x * 2], "%.2x", (unsigned char)src[x]);
}

/**
 * backupfs_from_hex
 * @dst: Buffer to take the bytes from src hex; must be at least of
 *       size (src_size / 2)
 * @src: Buffer to be converted from a hex string respresentation to raw value
 * @dst_size: size of dst buffer, or number of hex characters pairs to convert
 */
void backupfs_from_hex(char *dst, char *src, int dst_size)
{
	int x;
	char tmp[3] = { 0, };

	for (x = 0; x < dst_size; x++) {
		tmp[0] = src[x * 2];
		tmp[1] = src[x * 2 + 1];
		dst[x] = (unsigned char)simple_strtol(tmp, NULL, 16);
	}
}


int get_hashed_path(char *path,char *buf, int buflen)
{
	struct scatterlist sg;
	struct crypto_hash *tfm;
	struct hash_desc desc;
	int err=0,rc= 0,len,bufl=PAGE_SIZE;
	char *temp_buf;	
	
	temp_buf=(char*)kzalloc(bufl,GFP_KERNEL);
	
	tfm=crypto_alloc_hash("md5",0,CRYPTO_ALG_ASYNC);
	if(IS_ERR(tfm))
	{
		printk(KERN_EMERG"Failed to load transformation for MD5\n");
		err=PTR_ERR(tfm);
		goto alloc_error;
	}
	
	len=strlen(path);
	printk(KERN_EMERG"\npath=%s buf =%s len= %d",path,buf,len);
		
	//memset(buf,0,len);
	//memset(temp_buf,0,bufl);
	
	sg_init_one(&sg,(u8*) path , len);
	
	desc.tfm=tfm;
	desc.flags=0;
		
	rc=crypto_hash_init(&desc);
	if(rc)
	{
		printk(KERN_EMERG"\n error %d initi crypto",rc);
		err=rc;
		goto error;
	}
	
	rc=crypto_hash_update(&desc,&sg,len);
	if(rc)
	{
		printk(KERN_EMERG"\n error %d updtaing crypto",rc);
		err=rc;
		goto error;
	}
	
	rc=crypto_hash_final(&desc,temp_buf);	
	if(rc)
	{
		printk(KERN_EMERG"\n error %d final crypto",rc);		
		err=rc;
		goto error;
	}
	
	backupfs_to_hex(buf,temp_buf,32);
	buf[32]=0;
	
	//printk(KERN_EMERG"\ntemp_buf=%s buf =%s len= %d",temp_buf,buf,len);
	printk(KERN_EMERG"\nbuf =%s len= %d",buf,len);
	
	error:
	
	crypto_free_hash(tfm);
	
	alloc_error:
	
	kfree(temp_buf);
	
	
	return err;
	
}
/*
 * This method is used to update wrapfs modif_tym 
 * This method calls update_wrapfs_modif_tym of chardev to update the tym
 */
void get_updated_modif_tym(unsigned long data)
{
	int err;
	modif_tym = (__kernel_time_t) update_wrapfs_modif_tym();
	printk(KERN_EMERG"\n Updated modif tym = %ld",modif_tym);
	err = mod_timer( &update_timer, jiffies + msecs_to_jiffies(TIME_DELAY) );
	if (err)
	{
		printk("Error in mod_timer\n %d",err);
	}
}