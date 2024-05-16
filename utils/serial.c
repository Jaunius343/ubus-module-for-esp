#include "serial.h"

/* returns count of target devices, 0 if none are found*/
int serial_add_taget_devices_to_ubus_buf(struct sp_port **port_list, struct blob_buf *b)
{
	void *tbl;
	int i;
	int count = 0;

	for (i = 0; port_list[i] != NULL; ++i) {
		struct sp_port *port = port_list[i];
		int usb_vid, usb_pid;
		char hex_vid[5], hex_pid[5];

		char *port_name = sp_get_port_name(port);
		log_trace("Found port: %s", port_name);

		sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);
		log_trace("VID: %04X PID: %04X", usb_vid, usb_pid);

		sprintf(hex_vid, "%04X", usb_vid);
		sprintf(hex_pid, "%04X", usb_pid);

		if (usb_pid == TARGET_PID && usb_vid == TARGET_VID) {
			log_debug("Device found: %s", port_name);
			tbl = blobmsg_open_table(b, "table");
			blobmsg_add_string(b, "port", port_name);
			blobmsg_add_string(b, "vendor_id", hex_vid);
			blobmsg_add_string(b, "product_id", hex_pid);
			blobmsg_close_table(b, tbl);
			++count;
		}
	}

	log_info("target devices found: %d (from %d)", count, i);
	printf("\n");

	return count;
}

int serial_init_port(char *port_name, struct sp_port **port)
{
	int rc;
	int usb_vid, usb_pid;

	log_info("Looking for port %s.", port_name);
	rc = sp_get_port_by_name(port_name, port);
	if (rc != SP_OK) {
		check(rc);
		return rc;
	}

	sp_get_port_usb_vid_pid(*port, &usb_vid, &usb_pid);
	if (usb_pid != TARGET_PID || usb_vid != TARGET_VID) {
		sp_free_port(*port);
		check(SP_ERR_ARG);
		return SP_ERR_ARG;
	}

	log_info("Opening port.");
	rc = sp_open(*port, SP_MODE_READ_WRITE);
	if (rc != SP_OK) {
		check(rc);
		return rc;
	}

	log_info("Setting port to 9600 8N1, no flow control.");
	rc = sp_set_baudrate(*port, 9600);
	if (rc != SP_OK) {
		check(rc);
		return rc;
	}

	rc = sp_set_bits(*port, 8);
	if (rc != SP_OK) {
		check(rc);
		return rc;
	}

	rc = sp_set_parity(*port, SP_PARITY_NONE);
	if (rc != SP_OK) {
		check(rc);
		return rc;
	}

	rc = sp_set_stopbits(*port, 1);
	if (rc != SP_OK) {
		check(rc);
		return rc;
	}

	rc = sp_set_flowcontrol(*port, SP_FLOWCONTROL_NONE);
	if (rc != SP_OK) {
		check(rc);
		return rc;
	}

	return 0;
}

int serial_send_data(struct sp_port *port, char **response, char *msg)
{
	int size;
	unsigned int timeout = 2000;
	int result;

	// sprintf(data, "{\"action\": \"on\", \"pin\": %d}", pin);
	size = strlen(msg);

	log_trace("Sending '%s' (%d bytes) on port %s.", msg, size, sp_get_port_name(port));
	result = check(sp_blocking_write(port, msg, size, timeout));

	/* Check whether we sent all of the data. */
	if (result == size)
		log_trace("Sent %d bytes successfully.", size);
	else
		log_trace("Timed out, %d/%d bytes sent.", result, size);

	/* Allocate a buffer to receive data. */
	int buf_size = 128;
	char *buf    = malloc(buf_size + 1);

	/* Try to receive the data on the other port. */
	log_trace("Receiving %d bytes on port %s.", buf_size, sp_get_port_name(port));
	result = check(sp_blocking_read(port, buf, buf_size, timeout));
	log_trace("result: %d", result);

	/* Check whether we received the number of bytes we wanted. */
	if (result == buf_size)
		log_trace("Received %d bytes successfully.", buf_size);
	else
		log_trace("Timed out, %d/%d bytes received.", result, buf_size);

	/* Check if we received the same data we sent. */
	buf[result] = '\0';
	log_trace("Received '%s'.", buf);

	/* Free receive buffer. */
	// free(buf);
	*response = buf;

	return 0;
}

int serial_response_json(char *response, int *res, char **resp_msg)
{
	// Parse JSON string
	json_object *json = json_tokener_parse(response);
	if (json == NULL) {
		fprintf(stderr, "Error: Failed to parse JSON\n");
		// exit(EXIT_FAILURE);
		return -1;
	}

	// Extract values
	json_object *response_obj;
	if (!json_object_object_get_ex(json, "response", &response_obj)) {
		fprintf(stderr, "Error: 'response' key not found\n");
		json_object_put(json);
		// exit(EXIT_FAILURE);
		return -1;
	}
	*res = json_object_get_int(response_obj);

	json_object *msg_obj;
	if (!json_object_object_get_ex(json, "msg", &msg_obj)) {
		fprintf(stderr, "Error: 'msg' key not found\n");
		json_object_put(json);
		// exit(EXIT_FAILURE);
		return -1;
	}
	*resp_msg = strdup(json_object_get_string(msg_obj));

	json_object_put(json);

	return 0;
}

/* Helper function for error handling. */
int check(enum sp_return result)
{
	/* For this example we'll just exit on any error by calling abort(). */
	char *error_message;

	switch (result) {
	case SP_ERR_ARG:
		log_error("Error: Invalid argument.");
		// abort();
		break;
	case SP_ERR_FAIL:
		error_message = sp_last_error_message();
		log_error("Error: Failed: %s", error_message);
		sp_free_error_message(error_message);
		break;
		// abort();
	case SP_ERR_SUPP:
		log_error("Error: Not supported.");
		break;
		// abort();
	case SP_ERR_MEM:
		log_error("Error: Couldn't allocate memory.");
		break;
		// abort();
	case SP_OK:
	default:
		return result;
	}

	return result;
}
