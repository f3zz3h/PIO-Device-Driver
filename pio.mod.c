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
	{ 0x49b9b02b, "module_layout" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0x7ca9005, "usb_deregister" },
	{ 0x7791a751, "put_tty_driver" },
	{ 0xbaf37bee, "tty_unregister_driver" },
	{ 0xf1106312, "usb_register_driver" },
	{ 0x4ddd29cc, "tty_register_driver" },
	{ 0x19d5d880, "tty_set_operations" },
	{ 0x67b27ec1, "tty_std_termios" },
	{ 0x5a3729ec, "alloc_tty_driver" },
	{ 0xc275eb4a, "usb_register_dev" },
	{ 0x9f317dd2, "kmem_cache_alloc_trace" },
	{ 0x87d48e3e, "kmalloc_caches" },
	{ 0x37a0cba, "kfree" },
	{ 0x3c8729df, "usb_free_urb" },
	{ 0x7fa8dbd5, "mutex_unlock" },
	{ 0x71e3cecb, "up" },
	{ 0x999e5cbc, "usb_deregister_dev" },
	{ 0x68aca4ad, "down" },
	{ 0x9969c233, "dev_set_drvdata" },
	{ 0x54523cd9, "dev_get_drvdata" },
	{ 0x17769c25, "mutex_lock" },
	{ 0x27e1a049, "printk" },
	{ 0x544d6b90, "usb_kill_urb" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v09CAp5544d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "E146B6EB23D5957A64784AF");
