BUILD_DIR = build
TARGET_NAME = portal

ifeq ($(OS),Windows_NT)
    EXE = .exe
    RUN_CMD = $(BUILD_DIR)/$(TARGET_NAME)$(EXE)
else
    EXE =
    RUN_CMD = ./$(BUILD_DIR)/$(TARGET_NAME)$(EXE)
endif

all: $(BUILD_DIR)/$(TARGET_NAME)$(EXE)

$(BUILD_DIR)/$(TARGET_NAME)$(EXE): CMakeLists.txt
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	cmake --build $(BUILD_DIR)

run: all
	@$(RUN_CMD)

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all run clean
