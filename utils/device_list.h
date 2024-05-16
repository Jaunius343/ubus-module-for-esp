#include <stdlib.h>
#include <stdio.h>
#include <libserialport.h>
#include <string.h>

#include "log.h"

#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#define INITIAL_CAPACITY 10
#define CAP_STEP_SIZE	 10

#define TARGET_VID 0x10C4
#define TARGET_PID 0xEA60

struct device_data {
	char *port;
	char *vid;
	char *pid;
};

struct device_list_t {
	struct device_data **devices;
	int num_ports;
	int capacity;
};

struct device_list_t *device_list_initialize();
void device_list_resize(struct device_list_t *device_list);
void device_list_add(struct device_list_t *device_list, struct device_data port);
void device_list_add_CP210x(struct device_list_t *device_list, struct device_data *port);
void device_list_free(struct device_list_t *device_list);
void device_list_print(struct device_list_t *device_list);

#endif
