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
	{ 0x37013607, "mutex_unlock" },
	{ 0xfd6c037a, "usb_deregister_dev" },
	{ 0x7da7be30, "mutex_lock" },
	{ 0x10519fe3, "dev_get_drvdata" },
	{ 0x37a0cba, "kfree" },
	{ 0xf57a521b, "usb_free_urb" },
	{ 0xa8231eda, "usb_kill_urb" },
	{ 0xb0f36f9, "usb_register_dev" },
	{ 0xd498a420, "usb_driver_claim_interface" },
	{ 0xb3be75f6, "dev_set_drvdata" },
	{ 0xbb4d6341, "usb_alloc_urb" },
	{ 0xd61adcbd, "kmem_cache_alloc_trace" },
	{ 0x5cd9dbb5, "kmalloc_caches" },
	{ 0xdd703cff, "usb_ifnum_to_if" },
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0x27e1a049, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v09CAp5544d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "18E5A931B140140E7804613");
