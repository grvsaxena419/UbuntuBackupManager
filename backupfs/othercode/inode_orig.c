/*
 * Copyright (c) 1998-2011 Erez Zadok
 * Copyright (c) 2009	   Shrikar Archak
 * Copyright (c) 2003-2011 Stony Brook University
 * Copyright (c) 2003-2011 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "wrapfs.h"

int getrenamepath(struct dentry *dentry, struct path *lwpath,struct dentry **finalpath)
{
	struct dentry *lower_mountedon;
	char *newname,*namm;
	int len,err;
	struct nameidata lower_nd;
	
	printk(KERN_EMERG "Wrapfs: getrename path > Got request to delete %s",dentry->d_name.name);
	newname=(char*)kmalloc(buflen, GFP_KERNEL);
	
	memset(newname,0,buflen);
	strncpy(newname,dentry->d_name.name,buflen);
	strcat(newname,".unrm");
	len=strlen(newname);
	strcpy(namm,SAVEDIR);
	
	printk(KERN_EMERG "calling lookups");
	lower_mountedon=dentry->d_sb->s_root;
	printk(KERN_EMERG "Wrapfs: lowr_mntd on getrename path > %s d_inode=%p operations=%p  test opes=%p is root=%d inode_num=%d",lower_mountedon->d_name.name,lower_mountedon->d_inode,lower_mountedon->d_op,lower_mountedon->d_fsdata,IS_ROOT(lower_mountedon),lower_mountedon->d_inode->i_ino);
	err = vfs_path_lookup(dentry->d_sb->s_root, lwpath->mnt, namm, 0,&lower_nd);
	printk(KERN_EMERG "called lookups err=%d",err);
	
	if(err)
	{		
		printk(KERN_EMERG "Wrapfs: path lookup failed  = %s error code =%d",namm,err);
	}
	else
	{
		//printk(KERN_EMERG"wrapfs: no error in movepath  path > %s d_inode=%d operations=%p",movepath->d_name.name,movepath->d_inode,movepath->d_op);
		printk(KERN_EMERG"wrapfs: no error in movepjkhkjhk dentry =%p name=%s inode=%p op=%p",lower_nd.path.dentry,lower_nd.path.dentry->d_name.name,lower_nd.path.dentry->d_inode,lower_nd.path.dentry->d_op);
	}
	printk(KERN_EMERG"newname=%s",newname);
	printk(KERN_EMERG"Calling secnd lookup");
	err=vfs_path_lookup(lower_nd.path.dentry,lower_nd.path.mnt,newname,0,&lower_parent_path);
	printk(KERN_EMERG"Called secnd lookup");
	
	if(err)
	{
		printk(KERN_EMERG "Wrapfs: scnd lookup failed err=%d ",err);
		//printk(KERN_EMERG "Wrapfs: path lookup failed  = %s error code =%d",namm,err);
		return -EPERM;
	}
	else
	{
		//printk(KERN_EMERG"wrapfs: no error in movepath  path > %s d_inode=%d operations=%p",movepath->d_name.name,movepath->d_inode,movepath->d_op);
		printk(KERN_EMERG"wrapfs: no error in krn_path so lowerparent path=");
	}

	printk(KERN_EMERG" Returning from getrnme path");
	
	
	return 0;
	
	// old code 
	
	
	
	//wrapfs_get_lower_path(mountedon, &lower_path);
	//lower_mountedon = lower_path.dentry;
	mntd_parent=mountedon->d_parent;
		
	printk(KERN_EMERG "Wrapfs: mntd on getrename path > %s d_inode=%p operations=%p  test opes=%p is root=%d inode_num=%d",mountedon->d_name.name,mountedon->d_inode,mountedon->d_op,mountedon->d_fsdata,IS_ROOT(mountedon),mountedon->d_inode->i_ino);
	printk(KERN_EMERG "Wrapfs: other old getrename path > %s d_inode=%p operations=%p  test opes=%p is root=%d inode_num=%d",lwpath->mnt->mnt_mountpoint->d_name.name,lwpath->mnt->mnt_mountpoint->d_inode,lwpath->mnt->mnt_mountpoint->d_op,lwpath->mnt->mnt_mountpoint->d_fsdata,IS_ROOT(lwpath->mnt->mnt_mountpoint),lwpath->mnt->mnt_mountpoint->d_inode->i_ino);
	
	//printk(KERN_EMERG "Wrapfs: sure root getrename path > %s d_inode=%p operations=%p  test opes=%p is root=%d inode_num=%d",sure_root->d_name.name,sure_root->d_inode,sure_root->d_op,dentry->d_op,IS_ROOT(sure_root),sure_root->d_inode->i_ino);
	//wrapfs_get_lower_path(mntd_parent , &lower_parent_path);
	//mountedon=lower_parent_path.dentry;	
	
	retval=dentry_path_raw(mountedon,buf1,buflen);
	if(!IS_ERR(retval))
	{
		printk(KERN_EMERG "Wrapfs:root path = %s",retval);		
	}
	
	
	//finalpath=lookup_one_len(newname,mountedon,len);
	printk(KERN_EMERG "Wrapfs: found final path.. ");
	
	//if(IS_ERR(mountedon))
	//{
	//	printk(KERN_EMERG "Wrapfs:encountered error while finding directory final path");
	//	return PTR_ERR(finalpath);
	//}
	//*destdir=movepath->d_inode;
	//*destdir=mountedon->d_inode;
	//return finalpath;
	printk(KERN_EMERG "calling lookups");
	lower_mountedon=dentry->d_sb->s_root;
	printk(KERN_EMERG "Wrapfs: lowr_mntd on getrename path > %s d_inode=%p operations=%p  test opes=%p is root=%d inode_num=%d",lower_mountedon->d_name.name,lower_mountedon->d_inode,lower_mountedon->d_op,lower_mountedon->d_fsdata,IS_ROOT(lower_mountedon),lower_mountedon->d_inode->i_ino);
	//err = vfs_path_lookup(lwpath->mnt->mnt_mountpoint, lwpath->mnt, namm, 0,&lower_nd);
	err = vfs_path_lookup(dentry->d_sb->s_root, lwpath->mnt, namm, 0,&lower_nd);
	printk(KERN_EMERG "called lookups err=%d",err);
	//d_set_d_op(mountedon, &wrapfs_dops);
	//printk(KERN_EMERG "Wrapfs: mntd on getrename path > %s d_inode=%p operations=%p",mountedon->d_name.name,mountedon->d_inode,mountedon->d_op);
	//movepath=lookup_one_len(namm,sure_root,strlen(namm));
	
	if(err)
	{
		printk(KERN_EMERG "Wrapfs: path lookup failed  ");
		//printk(KERN_EMERG "Wrapfs: path lookup failed  = %s error code =%d",namm,err);
	}
	else
	{
		//printk(KERN_EMERG"wrapfs: no error in movepath  path > %s d_inode=%d operations=%p",movepath->d_name.name,movepath->d_inode,movepath->d_op);
		printk(KERN_EMERG"wrapfs: no error in movepjkhkjhk dentry =%p name=%s inode=%p op=%p",lower_nd.path.dentry,lower_nd.path.dentry->d_name.name,lower_nd.path.dentry->d_inode,lower_nd.path.dentry->d_op);
	}
	printk(KERN_EMERG"newname=%s",newname);
	printk(KERN_EMERG"Calling secnd lookup");
	err=vfs_path_lookup(lower_nd.path.dentry,lower_nd.path.mnt,newname,0,&lower_finalnd);
	printk(KERN_EMERG"Called secnd lookup");

	//err=kern_path("/sls-data",LOOKUP_FOLLOW | LOOKUP_DIRECTORY, &lower_parent_path);
	if(err)
	{
		printk(KERN_EMERG "Wrapfs: scnd lookup failed err=%d ",err);
		//printk(KERN_EMERG "Wrapfs: path lookup failed  = %s error code =%d",namm,err);
		//return -EPERM;
	}
	else
	{
		//printk(KERN_EMERG"wrapfs: no error in movepath  path > %s d_inode=%d operations=%p",movepath->d_name.name,movepath->d_inode,movepath->d_op);
		printk(KERN_EMERG"wrapfs: no error in krn_path so lowerparent path=");
	}

	printk(KERN_EMERG" Returning from getrnme path");
	
	return lower_finalnd.path.dentry;
	

	
	
	
	
	
	movepath=lookup_one_len(namm,mountedon,strlen(namm));
	//movepath=dentry->d_parent;
	
	wrapfs_get_lower_path(movepath, &lower_path);
	lower_dentry = lower_path.dentry;
	
	if(IS_ERR(movepath))
	{
		printk(KERN_EMERG "Wrapfs:encountered error while finding directory move path");
		return PTR_ERR(movepath);
	}
	retval=dentry_path_raw(lower_dentry,buf1,buflen);
	if(!IS_ERR(retval))
	{
		printk(KERN_EMERG "Wrapfs: movepath = %s",buf1);		
	}
	
	
	printk(KERN_EMERG "Wrapfs:new name = %s movepath=%s len=%d",newname,movepath->d_name.name,len);
	
	finalpath=lookup_one_len(newname,mountedon,len);
	printk(KERN_EMERG "Wrapfs: found final path.. ");
	
	if(IS_ERR(finalpath))
	{
		printk(KERN_EMERG "Wrapfs:encountered error while finding directory final path");
		return PTR_ERR(finalpath);
	}
	//*destdir=movepath->d_inode;
	*destdir=mountedon->d_inode;
	return finalpath;
	
	
	
	
}





// File Handling routines
// added by gs
/*static struct file* file_open(const char* path, int flags, int rights) 
{
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
    	err = PTR_ERR(filp);
    	return NULL;
    }
    return filp;
}

static void file_close(struct file* file) 
{
    filp_close(file, NULL);
}


static int file_read(struct file* file, loff_t *offset, unsigned char* data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());
	printk(KERN_EMERG"wrapfs: file read called offset=%lld ",*offset);
    ret = vfs_read(file, data, size, offset);
	

    set_fs(oldfs);
    return ret;
}


static int file_write(struct file* file, loff_t *offset, unsigned char* data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, offset);

    set_fs(oldfs);
    return ret;
}
*/



/*
int copy_file(struct path infile,struct path outfile)
{
	int err,ret,ctr;
	char *retval,*buf1,*name;
	struct file *in,*out;
	int buflen=PAGE_SIZE;
	loff_t read_pos = 0,write_pos=0;
	
	buf1=(char*)kzalloc(buflen, GFP_KERNEL);
	name=(char*)kzalloc(buflen, GFP_KERNEL);
	
	retval=dentry_path_raw(infile.dentry,buf1,buflen);
	//in=dentry_open(infile.dentry,infile.mnt,O_RDONLY,current_cred());
	
	strncpy(name,retval,buflen);
	printk(KERN_EMERG"wrapfs: Read file=%s name=%s",retval,name);
	
	in=file_open(name,O_RDONLY,0);
	//in=filp_open("/root/uu", O_RDONLY, 0);
	
	
	
	printk(KERN_EMERG"wrapfs: in=%p offset=%lld fop=%p filepos=%lld",in,in->f_pos,in->f_op,in->f_pos);
	printk(KERN_EMERG"wrapfs: infile dentry = %p, infile mnt=%p",infile.dentry,infile.mnt);
	if(IS_ERR(in))
	{
		printk(KERN_EMERG"wrapfs: Got error %ld opening file %s",PTR_ERR(in),infile.dentry->d_name.name);
		//return error
	}
	printk(KERN_EMERG"wrapfs: NO error till here");
	printk(KERN_EMERG"wrapfs: outfile dentry = %p, outfile mnt=%p",outfile.dentry,outfile.mnt);
	
	//dget(outfile.dentry);
    //mntget(outfile.mnt);
	
	retval=dentry_path_raw(outfile.dentry,buf1,buflen);
	//out=dentry_open(outfile.dentry,outfile.mnt,O_CREAT|O_WRONLY|O_TRUNC, current_cred());
	
	// cheack for error in retval
	out=file_open(retval,O_CREAT|O_WRONLY,0);
	if(IS_ERR(out))
	{
		printk(KERN_EMERG"wrapfs: Got error %ld in creating file %s",PTR_ERR(out),outfile.dentry->d_name.name);
		//return error
	}
	printk(KERN_EMERG"wrapfs: Now starting actual copy procedure...\n");
	ctr=0;
	
	
	while(1)
	{
		memset(buf1,0,buflen);
		ret=file_read(in,&read_pos,buf1,buflen);
		//ret = vfs_read(in, buf1, buflen, &read_pos);
		printk(KERN_EMERG"wrapfs: position of read = %lld,buf1=%s,ret=%d,ctr=%d",read_pos,buf1,ret,ctr);
		ret=file_write(out,&write_pos,buf1,ret);
		printk(KERN_EMERG"wrapfs: position of write = %lld,buf1=%s,ret=%d,ctr=%d",write_pos,buf1,ret,ctr);
		ctr++;
		if(ctr>=5)
			break;
	}
	
	printk(KERN_EMERG"wrapfs: ret = %d\n",ret);
	
	
	
	
	
	
	return 0;
}
*/
