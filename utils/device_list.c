#include "device_list.h"

struct device_list_t *device_list_initialize()
{
	struct device_list_t *device_list = malloc(sizeof(struct device_list_t));
	if (device_list == NULL) {
		log_error("Memory allocation failed");
		return NULL;
	}
	device_list->devices = malloc(INITIAL_CAPACITY * sizeof(struct device_list_t *));
	if (device_list->devices == NULL) {
		log_error("Memory allocation failed");
		return NULL;
	}
	device_list->num_ports = 0;
	device_list->capacity  = INITIAL_CAPACITY;
	return device_list;
}

void device_list_resize(struct device_list_t *device_list)
{
	device_list->capacity += CAP_STEP_SIZE;
	device_list->devices =
		realloc(device_list->devices, device_list->capacity * sizeof(struct device_data *));
	if (device_list->devices == NULL) {
		log_error("Memory allocation failed");
		return;
	}
}

// Function to create a new device
struct device_data *create_device(const char *port, const char *vid, const char *pid)
{
	struct device_data *device = malloc(sizeof(struct device_data));
	if (!device)
		return NULL;

	device->port = strdup(port);
	device->vid  = strdup(vid);
	device->pid  = strdup(pid);

	return device;
}

void device_list_add(struct device_list_t *device_list, struct device_data port)
{
	log_trace("adding port: %s", port.port);
	if (device_list->num_ports >= device_list->capacity) {
		device_list_resize(device_list);
	}

	struct device_data *port_copy = create_device(port.port, port.vid, port.pid);
	// log_trace("port copy: %s", port_copy->port);
	if (port_copy == NULL) {
		log_error("Failed to copy port");
		return;
	}

	device_list->devices[device_list->num_ports] = port_copy;
	device_list->num_ports++;
}

// void device_list_add_CP210x(struct device_list_t *device_list, struct device_data *port)
// {
// 	int usb_vid, usb_pid;
// 	sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);
// 	if (usb_vid == TARGET_VID && usb_pid == TARGET_PID) {
// 		device_list_add(device_list, port);
// 	}
// }

void device_list_free(struct device_list_t *device_list)
{
	if (device_list == NULL)
		return;
	for (int i = 0; i < device_list->num_ports; ++i) {
		free(device_list->devices[i]->port);
		free(device_list->devices[i]->pid);
		free(device_list->devices[i]->vid);
		free(device_list->devices[i]);
	}
	free(device_list->devices);
	free(device_list);
}

void device_list_print(struct device_list_t *device_list)
{
	if (device_list == NULL)
		return;
	printf("Port list:\n");
	for (int i = 0; i < device_list->num_ports; ++i) {
		const char *port_name = device_list->devices[i]->port;
		const char *vid	      = device_list->devices[i]->vid;
		const char *pid	      = device_list->devices[i]->pid;
		printf(" Port %d: %s %s %s\n", i + 1, port_name, vid, pid);
	}
	printf("Total ports: %d\n", device_list->num_ports);
}
