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
	{ 0x3b69997a, "module_layout" },
	{ 0xcd466ac0, "usb_deregister" },
	{ 0x9df96cd7, "usb_register_driver" },
	{ 0x5b453d9c, "usb_register_dev" },
	{ 0xb60aa18f, "usb_driver_claim_interface" },
	{ 0x9f2c6399, "usb_alloc_coherent" },
	{ 0x9cc570c3, "usb_alloc_urb" },
	{ 0x399c29ef, "kmem_cache_alloc_trace" },
	{ 0x62eb2c87, "kmalloc_caches" },
	{ 0x79ebfc26, "usb_ifnum_to_if" },
	{ 0x4dbf39e0, "mutex_unlock" },
	{ 0x71e3cecb, "up" },
	{ 0xafec1e42, "usb_deregister_dev" },
	{ 0x68aca4ad, "down" },
	{ 0x6c4f69e9, "mutex_lock" },
	{ 0x7a1dea15, "dev_set_drvdata" },
	{ 0x37a0cba, "kfree" },
	{ 0x70f9a21, "usb_free_urb" },
	{ 0x92f0769e, "usb_kill_urb" },
	{ 0x9c44f11e, "usb_submit_urb" },
	{ 0xc1afc80c, "dev_get_drvdata" },
	{ 0x4ae96d7c, "usb_find_interface" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x45d40463, "usb_control_msg" },
	{ 0xadaabe1b, "pv_lock_ops" },
	{ 0xd52bf1ce, "_raw_spin_lock" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xf6fad048, "usb_free_coherent" },
	{ 0x27e1a049, "printk" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v09CAp5544d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "4A3BDA4F4A925556A6D12D8");
