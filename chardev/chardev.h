#ifndef _WRAPFS_H_
#define _WRAPFS_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>


MODULE_LICENSE("GPL");

#define DEVICE_NAME "chardev"	/* Dev name as it appears in /proc/devices   */
#define SUCCESS 0
#define SIZE (30*(int)PAGE_SIZE) // assuming 30 Page_size as enough might require more if we encounter many buffer full messages

/* Ioctl constants */
# define UPDATE_MODIF_TYM 1

/* Methods required for functionality of wrapfs  */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *file,  unsigned int ioctl_num, unsigned long ioctl_param);

/* Exported Methods for wrapfs to use them */
extern int char_dev_buffer_insert(char msg[],int len);
extern int update_wrapfs_modif_tym(void);



#endif	
