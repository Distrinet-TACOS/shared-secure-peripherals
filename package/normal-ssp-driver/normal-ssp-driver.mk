################################################################################
#
# normal-ssp-driver
#
################################################################################

NORMAL_SSP_DRIVER_VERSION = 0.1
NORMAL_SSP_DRIVER_SITE = $(BR2_EXTERNAL_SHARED_SECURE_PERIPHERALS_PATH)/package/normal-ssp-driver
NORMAL_SSP_DRIVER_SITE_METHOD = local

NORMAL_SSP_DRIVER_DEPENDECIES = normal-controller

NORMAL_SSP_DRIVER_MODULE_MAKE_OPTS = \
	OPTEE_OS_SRC_DIR=$(OPTEE_OS_DIR) \
	NORMAL_CONTROLLER_SRC_DIR=$(NORMAL_CONTROLLER_DIR)


$(eval $(kernel-module))
$(eval $(generic-package))