#include <libubus.h>
#include <libubox/blobmsg_json.h>

#include "ubus.h"
#include "log.h"
#include "ubus_invoke.h"
#include "device_list.h"

// sudo ubus call esp on '{"port":"/dev/ttyUSB0","pin":5}'

int main(int argc, char **argv)
{
	struct ubus_context *ctx;
	uint32_t id;
	// struct device_data device = { 0 };
	struct device_list_t *device_list;

	device_list = device_list_initialize();

	// log_trace("device list from main: %s", device_list->devices[0]->port);

	// return 0;

	// log_set_level(LOG_DEBUG);
	log_set_level(LOG_TRACE);

	// uloop_init();

	log_trace("ubus connect");
	ctx = ubus_connect(NULL);
	if (!ctx) {
		log_error("Failed to connect to ubus\n");
		return -1;
	}

	// example to invoke devices
	if (ubus_lookup_id(ctx, "esp", &id) ||
	    ubus_invoke(ctx, id, "devices", NULL, devices_cb, device_list, 3000)) {
		log_error("can't request data from ubus");
		return -1;
	} else {
		// log_info("port: %s", device.port);
	}

	// example to invoke pin on/off
	struct blob_buf b = {};
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "port", "/dev/ttyUSB0");
	blobmsg_add_u32(&b, "pin", 5);

	if (ubus_lookup_id(ctx, "esp", &id) ||
	    ubus_invoke(ctx, id, "off", b.head, pin_cb, device_list, 3000)) {
		log_error("can't request data from ubus");
		return -1;
	} else {
		// log_info("port: %s", device.port);
	}

	blob_buf_free(&b);

	device_list_print(device_list);

	// ubus_add_uloop(ctx);
	// ubus_add_object(ctx, &esp_object);
	// uloop_run();

	ubus_free(ctx);
	// uloop_done();

	device_list_free(device_list);

	return 0;
}
