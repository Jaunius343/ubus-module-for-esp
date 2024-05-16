#include <libserialport.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "ubus.h"
#include "log.h"
#include "serial.h"

static int devices_get(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
		       const char *method, struct blob_attr *msg);

static int device_on(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
		     const char *method, struct blob_attr *msg);
static int device_off(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
		      const char *method, struct blob_attr *msg);

enum { ESP_PORT, ESP_PIN, __ESP_MAX };

static const struct blobmsg_policy esp_policy[__ESP_MAX] = {
	[ESP_PORT] = { .name = "port", .type = BLOBMSG_TYPE_STRING },
	[ESP_PIN]  = { .name = "pin", .type = BLOBMSG_TYPE_INT32 },
};

static const struct ubus_method esp_methods[] = { UBUS_METHOD("on", device_on, esp_policy),
						  UBUS_METHOD("off", device_off, esp_policy),
						  UBUS_METHOD_NOARG("devices", devices_get) };

static struct ubus_object_type esp_object_type = UBUS_OBJECT_TYPE("esp", esp_methods);

struct ubus_object esp_object = {
	.name	   = "esp",
	.type	   = &esp_object_type,
	.methods   = esp_methods,
	.n_methods = ARRAY_SIZE(esp_methods),
};

static int devices_get(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
		       const char *method, struct blob_attr *msg)
{
	// void *tbl;
	void *arr;
	struct sp_port **port_list;
	struct blob_buf b = {};

	blob_buf_init(&b, 0);
	arr = blobmsg_open_array(&b, "devices");

	enum sp_return result = sp_list_ports(&port_list);

	if (result != SP_OK) {
		log_error("sp_list_ports() failed!\n");
		return -1;
	}

	serial_add_taget_devices_to_ubus_buf(port_list, &b);

	blobmsg_close_array(&b, arr);

	sp_free_port_list(port_list);

	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return UBUS_STATUS_OK;
}

int handle_blob_message(struct blob_attr *msg, char **port_name, int *pin)
{
	struct blob_attr *tb[__ESP_MAX];
	blobmsg_parse(esp_policy, __ESP_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[ESP_PORT] || !tb[ESP_PIN])
		return UBUS_STATUS_INVALID_ARGUMENT;

	*port_name = blobmsg_get_string(tb[ESP_PORT]);
	*pin	   = blobmsg_get_u32(tb[ESP_PIN]);

	log_trace("port name: %s", *port_name);
	log_trace("pin: %d", *pin);

	return 0;
}

static int device_on(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
		     const char *method, struct blob_attr *msg)
{
	struct blob_buf b = {};
	struct sp_port *port;
	char *port_name;
	int pin;
	char *resp_json;
	char message[64];
	int res;
	char *resp_msg;
	int rc;

	rc = handle_blob_message(msg, &port_name, &pin);
	if (rc != 0)
		return rc;

	// send message to ESP
	rc = serial_init_port(port_name, &port);
	if (rc != SP_OK) {
		return UBUS_STATUS_CONNECTION_FAILED;
	}
	log_debug("Port name: %s\n", sp_get_port_name(port));

	sprintf(message, "{\"action\": \"on\", \"pin\": %d}", pin);
	serial_send_data(port, &resp_json, message);

	serial_response_json(resp_json, &res, &resp_msg);

	log_trace("blob_buf_init()");
	blob_buf_init(&b, 0);

	blobmsg_add_u32(&b, "result", res);
	blobmsg_add_string(&b, "msg", resp_msg);
	log_trace("blob_send_reply()");
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	log_debug("freeing port\n");
	sp_close(port);
	sp_free_port(port);

	free(resp_json);
	free(resp_msg);

	return UBUS_STATUS_OK;
}

static int device_off(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req,
		      const char *method, struct blob_attr *msg)
{
	struct blob_buf b = {};
	struct sp_port *port;
	char *port_name;
	int pin;
	char *resp_json;
	char message[64];
	int res;
	char *resp_msg;
	int rc;

	rc = handle_blob_message(msg, &port_name, &pin);
	if (rc != 0)
		return rc;

	// send message to ESP
	rc = serial_init_port(port_name, &port);
	if (rc != SP_OK) {
		return UBUS_STATUS_CONNECTION_FAILED;
	}
	log_debug("Port name: %s\n", sp_get_port_name(port));

	sprintf(message, "{\"action\": \"off\", \"pin\": %d}", pin);
	serial_send_data(port, &resp_json, message);

	serial_response_json(resp_json, &res, &resp_msg);

	log_trace("blob_buf_init()");
	blob_buf_init(&b, 0);

	blobmsg_add_u32(&b, "result", res);
	blobmsg_add_string(&b, "msg", resp_msg);
	log_trace("blob_send_reply()");
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	log_debug("freeing port");
	sp_close(port);
	sp_free_port(port);

	free(resp_json);
	free(resp_msg);

	return UBUS_STATUS_OK;
}