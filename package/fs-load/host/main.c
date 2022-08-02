// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2020, Open Mobile Platform LLC
 */

#include <err.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

#define PTA_LINUX_REBOOT_UUID                                                  \
	{                                                                      \
		0x9c694c03, 0xdb3a, 0x4653,                                    \
		{                                                              \
			0xab, 0xc3, 0xe3, 0x95, 0x52, 0x8e, 0x71, 0x1c         \
		}                                                              \
	}

int main(void)
{
	int i = 0;
	TEEC_Result res = TEEC_SUCCESS;
	TEEC_Context ctx = {};
	TEEC_Session sess = {};
	TEEC_Operation op = {};
	TEEC_UUID uuid = PTA_LINUX_REBOOT_UUID;
	uint32_t err_origin = 0;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code %#" PRIx32,
		     res);

	/* Open a session to the "plugin" TA */
	res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL,
			       NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1,
		     "TEEC_Opensession failed with code %#" PRIx32
		     "origin %#" PRIx32,
		     res, err_origin);

	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));
	op.paramTypes =
		TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);

	res = TEEC_InvokeCommand(&sess, 0, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1,
		     "TEEC_InvokeCommand failed with code %#" PRIx32
		     "origin %#" PRIx32,
		     res, err_origin);

	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

	return 0;
}
