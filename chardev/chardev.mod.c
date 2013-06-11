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
	{ 0x9a31bb74, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x405c1144, "get_seconds" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x27e3ef6b, "device_destroy" },
	{ 0x9cc5adfc, "__register_chrdev" },
	{ 0x37013607, "mutex_unlock" },
	{ 0xf432dd3d, "__init_waitqueue_head" },
	{ 0x64ce4311, "current_task" },
	{ 0xc45328ad, "mutex_lock_interruptible" },
	{ 0x27e1a049, "printk" },
	{ 0xd4f29297, "device_create" },
	{ 0x1000e51, "schedule" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xcf21d241, "__wake_up" },
	{ 0x5c8b5ce8, "prepare_to_wait" },
	{ 0xf3248a6e, "class_destroy" },
	{ 0xfa66f77c, "finish_wait" },
	{ 0x25827ecd, "__class_create" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "EA7E36977EB9493CD2B8E96");
