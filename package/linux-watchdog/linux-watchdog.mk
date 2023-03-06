################################################################################
#
# linux-watchdog
#
################################################################################

LINUX_WATCHDOG_VERSION = 0.1
LINUX_WATCHDOG_SITE = $(BR2_EXTERNAL_SHARED_SECURE_PERIPHERALS_PATH)/package/linux-watchdog
LINUX_WATCHDOG_SITE_METHOD = local

LINUX_WATCHDOG_DEPENDENCIES += optee-client optee-os linux

define LINUX_WATCHDOG_INSTALL_INIT_SYSV
        $(INSTALL) -m 0755 -D $(LINUX_WATCHDOG_PKGDIR)/S31linux-watchdog \
                $(TARGET_DIR)/etc/init.d/S31linux-watchdog
endef

$(eval $(cmake-package))