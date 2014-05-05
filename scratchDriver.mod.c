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
	{ 0x5358b4ed, "module_layout" },
	{ 0xcf0b783, "usb_deregister" },
	{ 0xf969cfd6, "usb_register_driver" },
	{ 0xb5ebeb1d, "usb_register_dev" },
	{ 0xa23f3078, "usb_driver_claim_interface" },
	{ 0xbd247324, "usb_alloc_coherent" },
	{ 0x1812662b, "usb_alloc_urb" },
	{ 0xf2b09e86, "kmem_cache_alloc_trace" },
	{ 0xfe1c648f, "kmalloc_caches" },
	{ 0x65163e9b, "usb_ifnum_to_if" },
	{ 0xf5c9e752, "usb_deregister_dev" },
	{ 0xe4ee7cc4, "dev_set_drvdata" },
	{ 0x37a0cba, "kfree" },
	{ 0x2489a72a, "usb_free_urb" },
	{ 0x37dd7d8e, "usb_kill_urb" },
	{ 0x8a8fb613, "dev_get_drvdata" },
	{ 0xee844374, "usb_find_interface" },
	{ 0xdfe19e37, "usb_bulk_msg" },
	{ 0x754d539c, "strlen" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x27e1a049, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v09CAp5544d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "0C1778C2E52C2523EAD243B");
