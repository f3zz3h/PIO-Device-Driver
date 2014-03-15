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
	{ 0x754c57a0, "usb_deregister" },
	{ 0x84e528ea, "usb_register_driver" },
	{ 0x37a0cba, "kfree" },
	{ 0xf57a521b, "usb_free_urb" },
	{ 0xa8231eda, "usb_kill_urb" },
	{ 0xb0f36f9, "usb_register_dev" },
	{ 0x27e1a049, "printk" },
	{ 0xd61adcbd, "kmem_cache_alloc_trace" },
	{ 0x5cd9dbb5, "kmalloc_caches" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v09CAp5544d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "A2CD5A7DF8CD8A7BDFFFD84");
