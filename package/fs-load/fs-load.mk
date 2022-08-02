################################################################################
#
# fs-load
#
################################################################################

FS_LOAD_VERSION = 0.1
FS_LOAD_SITE = $(BR2_EXTERNAL_SHARED_SECURE_PERIPHERALS_PATH)/package/fs-load
FS_LOAD_SITE_METHOD = local

FS_LOAD_DEPENDENCIES += optee-client optee-os

$(eval $(cmake-package))