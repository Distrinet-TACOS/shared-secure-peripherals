################################################################################
#
# normal-controller
#
################################################################################

NORMAL_CONTROLLER_VERSION = 0.1
NORMAL_CONTROLLER_SITE = $(BR2_EXTERNAL_SHARED_SECURE_PERIPHERALS_PATH)/package/normal-controller
NORMAL_CONTROLLER_SITE_METHOD = local

NORMAL_CONTROLLER_MODULE_MAKE_OPTS = OPTEE_OS_SRC_DIR=$(OPTEE_OS_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))