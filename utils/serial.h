#include <stdlib.h>
#include <stdio.h>
#include <libserialport.h>
#include <libubox/blobmsg_json.h>
#include <json-c/json.h>

#include "log.h"

#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#define TARGET_VID 0x10C4
#define TARGET_PID 0xEA60

int check(enum sp_return result);
int serial_add_taget_devices_to_ubus_buf(struct sp_port **port_list, struct blob_buf *b);
int serial_init_port(char *port_name, struct sp_port **port);
int serial_send_data(struct sp_port *port, char **response, char *msg);
int serial_response_json(char *response, int *res, char **resp_msg);

#endif