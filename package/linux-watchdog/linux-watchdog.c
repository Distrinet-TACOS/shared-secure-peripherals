#include <linux/init.h>
#include <linux/module.h>
#include <linux/tee_drv.h>
#include <asm/io.h>
#include <linux/reboot.h>
#include <asm/system_misc.h>
#include <linux/irqflags.h>
#include <linux/kexec.h>

#define DRIVER_NAME "Watchdog"

static const uuid_t sec_ssp_uuid =
	UUID_INIT(0xfc93fda1, 0x6bd2, 0x4e6a, 0x89, 0x3c, 0x12, 0x2f, 0x6c,
		  0x3c, 0x8e, 0x33);

static struct watchdog {
	u32 sess_id;
	struct tee_context *ctx;
} wd;

static void func(void)
{
	kernel_kexec();
}

static void call(void)
{
	struct tee_ioctl_invoke_arg inv_arg;
	struct tee_param param[4];
	int ret = 0;
	u32 sctlr;

	memset(&inv_arg, 0, sizeof(inv_arg));
	memset(&param, 0, sizeof(param));

	inv_arg.func = 2;
	inv_arg.session = wd.sess_id;
	inv_arg.num_params = 4;

	param[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_INPUT;
	param[0].u.value.a = func;

	ret = tee_client_invoke_func(wd.ctx, &inv_arg, param);
	if ((ret < 0) || (inv_arg.ret != 0)) {
		pr_err("PTA_WATCHDOG_RESET invoke error: %x.\n", inv_arg.ret);
		return;
	}
}

static void reset(void)
{
	pr_info("Invoking print function at linux-watchdog.c:a435ad\n");
	call();
}

static int open_session(const char *name, u32 *sess_id)
{
	struct tee_ioctl_open_session_arg sess_arg;
	struct tee_param param[4];
	int ret;

	memset(&sess_arg, 0, sizeof(sess_arg));
	export_uuid(sess_arg.uuid, &sec_ssp_uuid);
	sess_arg.clnt_login = TEE_IOCTL_LOGIN_PUBLIC;
	sess_arg.num_params = 4;

	memset(&param, 0, sizeof(param));

	ret = tee_client_open_session(wd.ctx, &sess_arg, param);
	if ((ret < 0) || (sess_arg.ret != 0)) {
		pr_err("tee_client_open_session failed for device %s, err: %x\n",
		       name, sess_arg.ret);
		return -EINVAL;
	}
	*sess_id = sess_arg.session;

	return 0;
}

static void close_session(void)
{
	tee_client_close_session(wd.ctx, wd.sess_id);
}

static int optee_ctx_match(struct tee_ioctl_version_data *ver, const void *data)
{
	if (ver->impl_id == TEE_IMPL_ID_OPTEE)
		return 1;
	else
		return 0;
}

static int create_context(void)
{
	wd.ctx = tee_client_open_context(NULL, optee_ctx_match, NULL, NULL);
	if (IS_ERR(wd.ctx))
		return -ENODEV;

	return 0;
}

static void destroy_context(void)
{
	tee_client_close_context(wd.ctx);
}

static int __init mod_init(void)
{
	int ret;

	if ((ret = create_context()) < 0) {
		pr_err("Could not create a context.\n");
		return ret;
	}
	if ((ret = open_session(DRIVER_NAME, &wd.sess_id)) < 0) {
		pr_err("Could not open a session\n.");
		return ret;
	}

	// Disable l2 cache
	// call();

	reset();

	return 0;
}

static void __exit mod_exit(void)
{
	close_session();
	destroy_context();
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Tom Van Eyck");
