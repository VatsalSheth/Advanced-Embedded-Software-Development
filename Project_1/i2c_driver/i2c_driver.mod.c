#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa88a79f7, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0xd9fdfc1d, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x699a98c, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xde8910bf, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x9b2aecc3, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x1c755610, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0xb70789e, __VMLINUX_SYMBOL_STR(__might_fault) },
	{ 0x91085136, __VMLINUX_SYMBOL_STR(i2c_master_recv) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x472d2a7c, __VMLINUX_SYMBOL_STR(i2c_master_send) },
	{ 0xe2e8065e, __VMLINUX_SYMBOL_STR(memdup_user) },
	{ 0x4f52b1bf, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xb81960ca, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0x11f74a9a, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xb084150d, __VMLINUX_SYMBOL_STR(i2c_get_adapter) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xf4a28464, __VMLINUX_SYMBOL_STR(i2c_put_adapter) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "27F259B85C00D28AE38BC3F");
