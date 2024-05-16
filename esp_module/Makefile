include $(TOPDIR)/rules.mk

PKG_NAME:=esp_module
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

TARGET_CFLAGS += -Og -ggdb3

include $(INCLUDE_DIR)/package.mk

define Package/esp_module
	CATEGORY:=Examples
	TITLE:=esp_module
	DEPENDS:= \
		+libubox \
		+libubus \
		+libblobmsg-json \
		+libjson-c \
		+libserialport 
endef

define Package/esp_module/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/esp_module $(1)/usr/bin/esp_module
	$(INSTALL_BIN) ./files/esp_module.init $(1)/etc/init.d/esp_module
endef

$(eval $(call BuildPackage,esp_module))
