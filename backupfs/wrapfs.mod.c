#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xe64ebc21, "module_layout" },
	{ 0x3900c844, "vfs_create" },
	{ 0x896b5e49, "kmem_cache_destroy" },
	{ 0xbbe3e4, "kmalloc_caches" },
	{ 0x3814b811, "fsstack_copy_inode_size" },
	{ 0xa7547c4c, "call_usermodehelper_exec" },
	{ 0xfad1192f, "generic_file_llseek" },
	{ 0xb85f3bbe, "pv_lock_ops" },
	{ 0xe6e95464, "mntget" },
	{ 0x298c5142, "vfs_readdir" },
	{ 0x64e615cb, "inode_permission" },
	{ 0xef80ef83, "char_dev_buffer_insert" },
	{ 0x1574c0b9, "del_timer" },
	{ 0xd0d8621b, "strlen" },
	{ 0xf83715d0, "d_set_d_op" },
	{ 0x4fdaef06, "iget5_locked" },
	{ 0x3ad4a939, "dget_parent" },
	{ 0x87a233f0, "vfs_link" },
	{ 0x86b425fd, "touch_atime" },
	{ 0x974bbbe6, "generic_delete_inode" },
	{ 0xeff07f97, "dput" },
	{ 0x477f168c, "dentry_open" },
	{ 0x99a50b1, "vfs_path_lookup" },
	{ 0xce0d981b, "vfs_mknod" },
	{ 0x9e1bdc28, "init_timer_key" },
	{ 0x38e3d0e7, "end_writeback" },
	{ 0xdaba5668, "mutex_unlock" },
	{ 0xbbce6eac, "vfs_fsync" },
	{ 0x39827ce8, "generic_read_dir" },
	{ 0x2ccc29c9, "igrab" },
	{ 0xf99aeb04, "vfs_symlink" },
	{ 0xf0f526f3, "mount_nodev" },
	{ 0x464db97d, "path_get" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x442385e, "mnt_drop_write" },
	{ 0x7d11c268, "jiffies" },
	{ 0xb356cb4, "truncate_setsize" },
	{ 0x1a328d57, "vfs_rmdir" },
	{ 0x7515e969, "unlock_rename" },
	{ 0xff39c273, "vfs_read" },
	{ 0xb053f079, "kern_path" },
	{ 0xfa07aba3, "current_task" },
	{ 0x50eedeb8, "printk" },
	{ 0x7ccab231, "d_rehash" },
	{ 0xb6ed1e53, "strncpy" },
	{ 0xb4390f9a, "mcount" },
	{ 0x6828eb03, "kmem_cache_free" },
	{ 0x83ca266, "lock_rename" },
	{ 0x8681b47d, "mutex_lock" },
	{ 0xce095088, "mod_timer" },
	{ 0x9c2dab0, "dentry_path_raw" },
	{ 0x264a2da8, "fput" },
	{ 0x61651be, "strcat" },
	{ 0x68e20477, "inode_init_once" },
	{ 0x8efedfb4, "kmem_cache_alloc" },
	{ 0xedd8462d, "mnt_want_write" },
	{ 0x9c983a69, "d_alloc" },
	{ 0xc8976a5c, "do_munmap" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0x534c20eb, "unlock_new_inode" },
	{ 0x221aedac, "d_drop" },
	{ 0x20a9539, "inode_newsize_ok" },
	{ 0x723e12ea, "update_wrapfs_modif_tym" },
	{ 0xa67d426c, "crypto_destroy_tfm" },
	{ 0x7ad93335, "vfs_statfs" },
	{ 0xde91ca00, "vfs_mkdir" },
	{ 0xfa0d9fc3, "inode_change_ok" },
	{ 0x90087979, "path_put" },
	{ 0xa4b69afd, "kmem_cache_alloc_trace" },
	{ 0x6443d74d, "_raw_spin_lock" },
	{ 0xa4425ded, "generic_show_options" },
	{ 0x1c795f25, "vfs_unlink" },
	{ 0x315f24e1, "kmem_cache_create" },
	{ 0x5ba47ba9, "register_filesystem" },
	{ 0xe5a4392d, "fsstack_copy_attr_all" },
	{ 0x7b48347, "d_lookup" },
	{ 0x13dde1af, "call_usermodehelper_setup" },
	{ 0x5c265cba, "sg_init_one" },
	{ 0xb374622e, "iput" },
	{ 0x37a0cba, "kfree" },
	{ 0x9592aecf, "unregister_filesystem" },
	{ 0x3f708d78, "init_special_inode" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0x701d0ebd, "snprintf" },
	{   0x98c6, "vfs_rename" },
	{ 0x3d214d00, "crypto_alloc_base" },
	{ 0x47792d4d, "notify_change" },
	{ 0x637417f8, "d_instantiate" },
	{ 0x237f746, "vfs_write" },
	{ 0xe914e41e, "strcpy" },
	{ 0xfece327d, "truncate_inode_pages" },
	{ 0xee182426, "generic_shutdown_super" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=chardev";


MODULE_INFO(srcversion, "A775E7E880E8C90DB8ADA49");
