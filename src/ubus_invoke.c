#include "ubus_invoke.h"

void devices_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct device_list_t *device_list = (struct device_list_t *)req->priv;
	struct blob_attr *arr[__LIST_MAX];
	// struct blob_attr *arr;
	struct blob_attr *cur;
	struct blob_attr *tb[__PORT_MAX];

	blobmsg_parse(device_array_policy, __LIST_MAX, arr, blob_data(msg), blob_len(msg));

	if (!arr[DEVICES_LIST]) {
		log_error("no data received");
		return;
	}

	log_info("reading port list");

	int rem;
	blobmsg_for_each_attr (cur, arr[DEVICES_LIST], rem) {
		struct device_data device = {};

		blobmsg_parse(device_data_policy, __PORT_MAX, tb, blobmsg_data(cur), blobmsg_len(cur));

		if (!tb[PORT_NAME] || !tb[PORT_VENDOR] || !tb[PORT_PRODUCT])
			continue;

		device.port = blobmsg_get_string(tb[PORT_NAME]);
		device.vid  = blobmsg_get_string(tb[PORT_VENDOR]);
		device.pid  = blobmsg_get_string(tb[PORT_PRODUCT]);
		log_info("found port: %s", device.port);

		device_list_add(device_list, device);
	}
}

void pin_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	// Convert to plain JSON
	log_trace("inside device_cb");
	char *json_str = blobmsg_format_json(msg, true);
	if (json_str) {
		log_debug("payload: \'%s\'", json_str);
		free(json_str);
	}
	return;
}
