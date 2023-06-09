CC = g++

BUILD_DIR ?= build
SRC_DIRS ?= src
LIB_DIRS ?= lib
DEST_DIR ?= /usr/local/bin

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS := -lpython3.11
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
LDFLAGS := $(shell find $(LIB_DIRS) -name *.o)
LDFLAGS += $(shell find $(LIB_DIRS) -name *.a)
LIBS := $(shell find $(LIB_DIRS)/*/$(SRC_DIRS) -name *.cpp)

CPPFLAGS ?= -Wall

TARGET_EXEC ?= hamza

all: bin/$(TARGET_EXEC)

bin/$(TARGET_EXEC): $(OBJS)
	@echo "Linking..."
	$(MKDIR_P) $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS) -I/usr/include/python3.11/ -lpython3.11

$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo "Making Objects..."
	$(MKDIR_P) $(dir $@)
	$(CC) $(LDFLAGS) $(CPPFLAGS) -c $< -o $@

install: all
	mkdir -p ${DEST_DIR}
	cp -f stdstreams ${DEST_DIR}/${TARGET_EXEC}
	chmod 755 ${DEST_DIR}/${TARGET_EXEC}

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
