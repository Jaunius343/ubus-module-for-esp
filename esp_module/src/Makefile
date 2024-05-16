INCLUDE_DIRS = -I $(UTILS_DIR) -I $(LIBS_DIR)

# -D_XOPEN_SOURCE=700 is used because compile standard is defined as c11
# and ocmpiler throws an error for libubox otherwise
CFLAGS = -Wall -std=c11 $(INCLUDE_DIRS) -ggdb3 -D_XOPEN_SOURCE=700
LDFLAGS = -lblobmsg_json \
		-lubox \
		-lubus \
		-lserialport \
		-ljson-c

SRC_DIR := src
OBJ_DIR := obj
UTILS_DIR := utils
LIBS_DIR := libs

SRCS := $(wildcard $(SRC_DIR)/*.c)
UTILS := $(wildcard $(UTILS_DIR)/*.c)
LIBS := $(wildcard $(LIBS_DIR)/*.c)

# test:
# 	echo srcs $(SRCS)
# 	echo utils $(UTILS)

OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
UTIL_OBJS := $(patsubst $(UTILS_DIR)/%.c,$(OBJ_DIR)/%.o,$(UTILS))
LIB_OBJS := $(patsubst $(LIBS_DIR)/%.c,$(OBJ_DIR)/%.o,$(LIBS))

EXECUTABLE := esp_module

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(UTIL_OBJS) $(LIB_OBJS) $(OBJS) 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(UTILS_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIBS_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@




clean:
	rm -f $(OBJ_DIR)/*.o $(EXECUTABLE)

