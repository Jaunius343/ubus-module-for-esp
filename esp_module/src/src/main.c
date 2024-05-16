#include <libubus.h>

#include "ubus.h"
#include "log.h"

// sudo ubus call esp on '{"port":"/dev/ttyUSB0","pin":5}'

int main(int argc, char **argv)
{
	struct ubus_context *ctx;

	// log_set_level(LOG_DEBUG);
	log_set_level(LOG_TRACE);

	uloop_init();

	ctx = ubus_connect(NULL);
	if (!ctx) {
		fprintf(stderr, "Failed to connect to ubus\n");
		return -1;
	}

	ubus_add_uloop(ctx);
	ubus_add_object(ctx, &esp_object);
	uloop_run();

	ubus_free(ctx);
	uloop_done();

	return 0;
}
