################################################################################
#
# linux-watchdog
#
################################################################################

LINUX_WATCHDOG_VERSION = 0.1
LINUX_WATCHDOG_SITE = $(BR2_EXTERNAL_SHARED_SECURE_PERIPHERALS_PATH)/package/linux-watchdog
LINUX_WATCHDOG_SITE_METHOD = local

# LINUX_WATCHDOG_MODULE_MAKE_OPTS = OPTEE_OS_SRC_DIR=$(OPTEE_OS_DIR)
# LINUX_WATCHDOG_DEPENDENCIES += optee-os
LINUX_WATCHDOG_DEPENDENCIES += linux

$(eval $(kernel-module))
$(eval $(generic-package))