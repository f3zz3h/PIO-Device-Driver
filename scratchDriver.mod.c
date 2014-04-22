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
	{ 0x6a91b2bc, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x26a2a864, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0x9c82223b, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xfb5d08f5, __VMLINUX_SYMBOL_STR(usb_free_urb) },
	{ 0x6e982e02, __VMLINUX_SYMBOL_STR(usb_kill_urb) },
	{ 0xf5040d0a, __VMLINUX_SYMBOL_STR(usb_register_dev) },
	{ 0x92f73c50, __VMLINUX_SYMBOL_STR(usb_driver_claim_interface) },
	{ 0xc49885f, __VMLINUX_SYMBOL_STR(usb_alloc_coherent) },
	{ 0x5bcd972d, __VMLINUX_SYMBOL_STR(usb_alloc_urb) },
	{ 0x439a4a19, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xf5769951, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xafacb69e, __VMLINUX_SYMBOL_STR(usb_ifnum_to_if) },
	{ 0xd1a87653, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x697d5361, __VMLINUX_SYMBOL_STR(usb_deregister_dev) },
	{ 0x4582cd80, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0x1e9079e7, __VMLINUX_SYMBOL_STR(dev_set_drvdata) },
	{ 0xf72f16d8, __VMLINUX_SYMBOL_STR(usb_submit_urb) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xe6a13900, __VMLINUX_SYMBOL_STR(dev_get_drvdata) },
	{ 0xbc9c2b16, __VMLINUX_SYMBOL_STR(usb_find_interface) },
	{ 0xec3ae7e8, __VMLINUX_SYMBOL_STR(usb_free_coherent) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v09CAp5544d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "D7F5DFBEC33207AA06A1B4D");
