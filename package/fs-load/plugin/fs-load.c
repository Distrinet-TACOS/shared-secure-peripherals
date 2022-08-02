// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2020, Open Mobile Platform LLC
 */

#include <stddef.h>
#include <tee_plugin_method.h>
#include <teec_trace.h>
#include <string.h>
#include <stdio.h>
#include <zlib.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

/*
 * OPTEE has access to the plugin by the UUID
 */
#define FS_LOAD_PLUGIN_UUID                                                    \
	{                                                                      \
		0xb9b4b4c7, 0x983f, 0x497a,                                    \
		{                                                              \
			0x9f, 0xdc, 0xf5, 0xdf, 0x86, 0x4f, 0x29, 0xc4         \
		}                                                              \
	}

#define FS_LOAD_KERNEL_IMAGE 0
#define FS_LOAD_CLEAN 1

static TEEC_Result fs_load_plugin_init(void)
{
	IMSG("Initing plugin");
	return TEEC_SUCCESS;
}

static TEEC_Result load_kernel_image(void *buffer, size_t buf_size,
				     size_t *ta_size)
{
	char fname[PATH_MAX] = "/boot/zImage";
	FILE *file = NULL;
	size_t s = 0;

	IMSG("Looking for kernel image");

	file = fopen(fname, "r");
	if (file == NULL) {
		EMSG("Failed to open kernel image");
		return TEEC_ERROR_ITEM_NOT_FOUND;
	}

	if (fseek(file, 0, SEEK_END) != 0) {
		fclose(file);
		return TEEC_ERROR_NO_DATA;
	}

	IMSG("Determining kernel image size");

	s = ftell(file);
	if (s > buf_size || !buffer) {
		IMSG("Returning correct buffer size");
		*ta_size = s;
		fclose(file);
		return TEEC_ERROR_SHORT_BUFFER;
	}

	IMSG("Reading kernel image into buffer");

	if (fseek(file, 0, SEEK_SET) != 0) {
		fclose(file);
		return TEEC_ERROR_NO_DATA;
	}

	if (s != fread(buffer, 1, s, file)) {
		EMSG("Failed to read kernel image");
		fclose(file);
		return TEEC_ERROR_NO_DATA;
	}

	IMSG("Successfully read buffer");

	*ta_size = s;
	fclose(file);
	return TEEC_SUCCESS;
}

static TEEC_Result clean() {
	return TEEC_SUCCESS;
}

static TEEC_Result fs_load_plugin_invoke(unsigned int cmd, unsigned int sub_cmd,
					 void *data, size_t data_len,
					 size_t *out_len)
{
	IMSG("plugin invoked");
	switch (cmd) {
	case FS_LOAD_KERNEL_IMAGE:
		return load_kernel_image(data, data_len, out_len);
	case FS_LOAD_CLEAN:
		return clean();
	default:
		break;
	}

	return TEEC_ERROR_NOT_SUPPORTED;
}

struct plugin_method plugin_method = {
	"fs-load",
	FS_LOAD_PLUGIN_UUID,
	fs_load_plugin_init, /* can be NULL */
	fs_load_plugin_invoke,
};
