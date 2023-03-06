#include <err.h>
#include <inttypes.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

#define DRIVER_NAME "Watchdog"
#define PTA_WATCHDOG_UUID                                                      \
	{                                                                      \
		0xfc93fda1, 0x6bd2, 0x4e6a,                                    \
		{                                                              \
			0x89, 0x3c, 0x12, 0x2f, 0x6c, 0x3c, 0x8e, 0x33         \
		}                                                              \
	}
#define PTA_WATCHDOG_SETUP 0
#define PTA_WATCHDOG_UPDATE 1

int main(void)
{
	int i = 0;
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = PTA_WATCHDOG_UUID;
	uint32_t err_origin;

	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS) {
		errx(1, "TEEC_InitializeContext failed with code %#" PRIx32,
		     res);
		return res;
	}

	res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL,
			       NULL, &err_origin);
	if (res != TEEC_SUCCESS) {
		errx(1,
		     "TEEC_Opensession failed with code %#" PRIx32
		     " origin %#" PRIx32,
		     res, err_origin);
		return res;
	}

	memset(&op, 0, sizeof(op));
	op.paramTypes =
		TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
	res = TEEC_InvokeCommand(&sess, PTA_WATCHDOG_SETUP, &op, &err_origin);
	if (res != TEEC_SUCCESS) {
		errx(1,
		     "TEEC_InvokeCommand failed with code %#" PRIx32
		     " origin %#" PRIx32,
		     res, err_origin);
		return res;
	}

	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

	return 0;
}