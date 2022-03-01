################################################################################
#
# normal-controller
#
################################################################################

BENCHMARKER_VERSION = 0.1
BENCHMARKER_SITE = $(BR2_EXTERNAL_SHARED_SECURE_PERIPHERALS_PATH)/package/benchmarker
BENCHMARKER_SITE_METHOD = local

BENCHMARKER_MODULE_MAKE_OPTS = OPTEE_OS_SRC_DIR=$(OPTEE_OS_DIR)

BENCHMARKER_DEPENDECIES += optee-os
BENCHMARKER_DEPENDECIES += linux

$(eval $(kernel-module))
$(eval $(generic-package))