MODULE   		:= smartcfg
CROSS_COMPILE 	:= arm-linux-gnueabihf-
CC 				:= $(CROSS_COMPILE)gcc
AR 				:= $(CROSS_COMPILE)ar

BUILD_TOP		:= $(shell pwd)
OUT_DIR    		= $(BUILD_TOP)/out
OBJ_DIR     	= $(OUT_DIR)/obj
BIN_DIR     	= $(OUT_DIR)/bin
INCLUDE_DIR 	= $(OUT_DIR)/include
LIB_DIR     	= $(OUT_DIR)/lib
SRC_DIR     	= $(BUILD_TOP)/src
EXAMPLE			= $(BUILD_TOP)/example
HEADER		    = $(SRC_DIR)/smartcfg.h 

OBJ_FILE 		= $(wildcard $(OBJ_DIR)/*.o)
CFLAGS 			= -I$(SRC_DIR) -lpthread -lrt -Wall -fPIC -g

vpath %.c $(SRC_DIR)

OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
DEMO = $(patsubst $(EXAMPLE)/%.c, $(BIN_DIR)/%, $(wildcard $(EXAMPLE)/*.c))

.PHONY: all clean OUT_PATH 

all: clean OUT_PATH $(OBJ) $(DEMO) SMART_LIBS

OUT_PATH:
	[ -d $(OUT_DIR) ] || mkdir -p $(OUT_DIR)
	[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	[ -d $(INCLUDE_DIR) ] || mkdir -p $(INCLUDE_DIR)
	[ -d $(LIB_DIR) ] || mkdir -p $(LIB_DIR)

SMART_LIBS:
	$(AR) rcs $(LIB_DIR)/lib$(MODULE).a $(OBJ_FILE)
	$(CC) -shared -o $(LIB_DIR)/lib$(MODULE).so $(OBJ_FILE)
	cp $(HEADER) $(INCLUDE_DIR)

$(BIN_DIR)/%: $(EXAMPLE)/%.c
	$(CC) -o $@ $^ $(OBJ_FILE) $(CFLAGS)

$(OBJ_DIR)/%.o: %.c
	@echo "Compiling $< to $@"
	$(CC) -c $^ -o $@ $(CFLAGS) $(ALGOLIBS) $(LDFLAGS)

clean:
	-$(RM) -rf $(OUT_DIR) 
