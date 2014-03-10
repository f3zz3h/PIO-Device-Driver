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
	{ 0x3ec8886f, "param_ops_int" },
	{ 0xcd466ac0, "usb_deregister" },
	{ 0xbcf01c62, "put_tty_driver" },
	{ 0x5ff32c45, "tty_unregister_driver" },
	{ 0x9df96cd7, "usb_register_driver" },
	{ 0x2269ac4e, "tty_register_driver" },
	{ 0x6de651b, "tty_set_operations" },
	{ 0x67b27ec1, "tty_std_termios" },
	{ 0x3a410a75, "alloc_tty_driver" },
	{ 0x5b453d9c, "usb_register_dev" },
	{ 0x399c29ef, "kmem_cache_alloc_trace" },
	{ 0x62eb2c87, "kmalloc_caches" },
	{ 0x37a0cba, "kfree" },
	{ 0x70f9a21, "usb_free_urb" },
	{ 0x4dbf39e0, "mutex_unlock" },
	{ 0x71e3cecb, "up" },
	{ 0xafec1e42, "usb_deregister_dev" },
	{ 0x68aca4ad, "down" },
	{ 0x7a1dea15, "dev_set_drvdata" },
	{ 0xc1afc80c, "dev_get_drvdata" },
	{ 0x6c4f69e9, "mutex_lock" },
	{ 0x27e1a049, "printk" },
	{ 0x92f0769e, "usb_kill_urb" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v09CAp5544d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "DF9AD9668260D6665AC2704");
