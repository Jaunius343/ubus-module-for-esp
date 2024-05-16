#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "log.h"
#include "device_list.h"

#ifndef UBUS_INVOKE_H_INCLUDED
#define UBUS_INVOKE_H_INCLUDED

enum {
	PORT_NAME,
	PORT_VENDOR,
	PORT_PRODUCT,
	__PORT_MAX,
};

enum {
	DEVICE_TABLE,
	__TABLE_MAX,
};

enum {
	DEVICES_LIST,
	__LIST_MAX,
};

static const struct blobmsg_policy device_data_policy[__PORT_MAX] = {
	[PORT_NAME]    = { .name = "port", .type = BLOBMSG_TYPE_STRING },
	[PORT_VENDOR]  = { .name = "vendor_id", .type = BLOBMSG_TYPE_STRING },
	[PORT_PRODUCT] = { .name = "product_id", .type = BLOBMSG_TYPE_STRING },
};

static const struct blobmsg_policy device_table_policy[__TABLE_MAX] = {
	[DEVICE_TABLE] = { .name = "devices", .type = BLOBMSG_TYPE_TABLE },
};

static const struct blobmsg_policy device_array_policy[__LIST_MAX] = {
	[DEVICES_LIST] = { .name = "devices", .type = BLOBMSG_TYPE_ARRAY },
};

void devices_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void pin_cb(struct ubus_request *req, int type, struct blob_attr *msg);

#endif