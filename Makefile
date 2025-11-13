BUILD_DIR = build
OUTPUT_DIR = $(BUILD_DIR)/output
INSTALL_DIR = $(BUILD_DIR)/install


.PHONY: clean build

build:
	@echo "Starting build process... $(shell nproc) cores"
	cmake -B $(BUILD_DIR) -DDATABASE_ARMORY_BUILD_TESTS=ON -DDATABASE_ARMORY_BUILD_EXAMPLE=ON
	cmake --build $(BUILD_DIR) -j$(shell nproc)
	cp $(BUILD_DIR)/example/camera/camera_example $(OUTPUT_DIR)


clean:
	rm -rf $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)
	rm -rf $(OUTPUT_DIR)
	mkdir -p $(OUTPUT_DIR)

rebuild: clean build
