# include "chardev.h"


static char msgbuffer[SIZE];
static int start_pos,current_pos,count_data;

/* 
 * Global variables are declared as static, so are global within the file. 
 */

static int major;		/* Major number assigned to our device driver */
static int Device_Open = 0;	/* Is device open?  
				 * Used to prevent multiple access to device */
//static char msg[BUF_LEN];	/* The msg the device will give when asked */

static struct class *class_chardev;
static struct device *dev_chardev;
static wait_queue_head_t reader_queue;
static int modif_tym=0;
static DEFINE_MUTEX(buffer_lock);

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
	.open = device_open,
	.release = device_release
};

int init_module(void)
{
	void *ptr_err;
	if ((major = register_chrdev(0, DEVICE_NAME, &fops)) < 0)
		return major;

	class_chardev = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(ptr_err = class_chardev))
		goto err2;

	dev_chardev = device_create(class_chardev, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(ptr_err = dev_chardev))
		goto err;

	/* struct kobject *play_with_this = &dev_foo->kobj; */
	start_pos=0;
	current_pos=0;
	count_data=0;
	init_waitqueue_head(&reader_queue);
	
	return 0;
err:
	class_destroy(class_chardev);
err2:
	unregister_chrdev(major, DEVICE_NAME);
	return PTR_ERR(ptr_err);
}

void cleanup_module(void)
{
	device_destroy(class_chardev, MKDEV(major, 0));
	class_destroy(class_chardev);
	return unregister_chrdev(major, DEVICE_NAME);
}



int char_dev_buffer_insert(char msg[],int len)
{
	int j, err = 0;
	//printk(KERN_EMERG"\nchardev: this is test export_symbol %s\n", __FUNCTION__);
	
	printk(KERN_EMERG"\nchardev: buf_ins start_pos=%d,currentpos=%d count_data=%d length=%d SIZE=%d",start_pos,current_pos,count_data,len,SIZE);
	
	// lock this code 
	
	err = mutex_lock_interruptible(&buffer_lock);
	
	if(err!=0)
	{
		goto not_unlock;
	}
	
	if(len>SIZE-count_data)
	{
		printk(KERN_EMERG"\nchardev: BUG: **Buffer is full**  start_pos=%d,currentpos=%d count_data=%d",start_pos,current_pos,count_data);
		// release lock
		len=-1;
		goto unlock;
	}
		
	
	for(j=0;j<len;j++)
	{
		msgbuffer[current_pos]=msg[j];
		//printk(KERN_EMERG"\nchardev: currentpos=%d msg[%d]=%c msgbuffer[%d]=%c",current_pos,j,msg[j],current_pos,msgbuffer[current_pos]);
		current_pos=(current_pos+1)%SIZE;
		if(current_pos==start_pos)
		{
			printk(KERN_EMERG"chardev: *Overwrite* starts not possible");
		}
		count_data++;
	}
	
	// release all locks
	
	//strcpy(msgbuffer+current_pos,msg);
	//current_pos+=len;
	printk(KERN_EMERG"\nchardev: buf_ins start_pos=%d,currentpos=%d count_data=%d ",start_pos,current_pos,count_data);
	
	//printk(KERN_DEBUG "process %i (%s) awakening the readers...\n", current->pid, current->comm);
	
	unlock:
	mutex_unlock(&buffer_lock);
	wake_up_interruptible(&reader_queue);
	
	not_unlock:
	
	return len;
}
EXPORT_SYMBOL(char_dev_buffer_insert);


int update_wrapfs_modif_tym(void)
{
	if(modif_tym==0)
	{
		return (int)get_seconds();
	}
	else
	{
		return modif_tym;
	}
}
EXPORT_SYMBOL(update_wrapfs_modif_tym);

/*
 * Methods
 */

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/chardev"
 */
static int device_open(struct inode *inode, struct file *file)
{	

	if (Device_Open)
		return -EBUSY;

	printk("\nchardev: open start_pos=%d,currentpos=%d count=%d",start_pos,current_pos,count_data);
	Device_Open++;

	//try_module_get(THIS_MODULE);

	return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	
	printk("\nchardev: cloased start_pos=%d,currentpos=%d",start_pos,current_pos);
	//module_put(THIS_MODULE);

	return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0,j, err = 0;
	
	
	printk("\nchardev: read start_pos=%d,currentpos=%d readablecount=%d length=%d",start_pos,current_pos,count_data,length);
	
	// lock start
	printk("\nchardev: read -- trying mutex_lock ");
	err = mutex_lock_interruptible(&buffer_lock);
	if(err!=0)
	{
		goto not_unlock;
	}
	printk("\nchardev: read -- mutex_locked ");
	if(count_data==0)
	{		
		printk("\nchardev: BUG: **buffer is empty**");
		printk("\nchardev: trying to sleep process till data is available");
		//printk(KERN_DEBUG "process %i (%s) going to sleep\n", current->pid, current->comm);
		mutex_unlock(&buffer_lock);
		printk("\nchardev: read -- mutex_unlocked ");
		err = wait_event_interruptible(reader_queue, count_data != 0);
		printk("\nchardev: read -- wait event complete ");
		//printk(KERN_DEBUG "awoken %i (%s)\n", current->pid, current->comm);
		if(err<0)
		{
			printk(KERN_EMERG"chardev: Process Interrupted need to return error");
			goto not_unlock;			
		}
		
		printk("\nchardev: read -- trying mutex_lock1 ");
		err = mutex_lock_interruptible(&buffer_lock);
		if(err!=0)
		{
			goto not_unlock;
		}
		printk("\nchardev: read -- mutex_locked1 ");
	}	

	/* 
	 * Actually put the data into the buffer 
	 */
	for(j=0;j<length&&count_data>0;j++)
	{
		buffer[j]=msgbuffer[start_pos];
		//printk(KERN_EMERG"\nchardev: startpos=%d msgbuffer[%d]=%c buffer[%d]=%c",start_pos,j,msgbuffer[start_pos],start_pos,buffer[j]);
		start_pos=(start_pos+1)%SIZE;
		count_data--;
		bytes_read++;
	}
	
	//if(length>(current_pos-start_pos))
	//{
		//strncpy(buffer,msgbuffer,(current_pos-start_pos));
		//bytes_read=current_pos-start_pos;
		//start_pos=current_pos;
		//return bytes_read;
	//}
	//else
	//{
		//strncpy(buffer,msgbuffer,length);
		//bytes_read=length;
		//start_pos=start_pos+length;
		//return bytes_read;
	//}
	

	
	// release all locks
	mutex_unlock(&buffer_lock);
	printk("\nchardev: read -- mutex_unlocked2 ");
	not_unlock:
	
	printk(KERN_EMERG"chardev: read ended start_pos=%d,currentpos=%d readablecount=%d bytes_read=%d",start_pos,current_pos,count_data,bytes_read);
	return bytes_read;
}

/*  
 * Called when a process writes to dev file: echo "hi" > /dev/hello 
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
	return -EINVAL;
}



/* 
 * This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function.
 *
 */
long device_ioctl(struct file *file,  unsigned int ioctl_num, unsigned long ioctl_param)
{
	long new_modif_tym;
	/* 
	 * Switch according to the ioctl called 
	 */	
	printk(KERN_ALERT"chardev: ioctl called num = %d param = %ld ",ioctl_num,ioctl_param);
	switch (ioctl_num) 
	{
		case UPDATE_MODIF_TYM:			
			//Receive new modif tym in the ioctl param			
			new_modif_tym = ioctl_param;

			if(new_modif_tym!=0)
				modif_tym = new_modif_tym;

			//device_write(file, (char *)ioctl_param, i, 0);
			break;
	}
	
	return 0;
}