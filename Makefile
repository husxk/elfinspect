
CMAKE ?= cmake
BUILD_DIR ?= build

.PHONY: all linux clean run

all: linux

linux:
	$(CMAKE) -S . -B $(BUILD_DIR)
	$(CMAKE) --build $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

run:
	./$(BUILD_DIR)/elfinspect ./$(BUILD_DIR)/elfinspect

