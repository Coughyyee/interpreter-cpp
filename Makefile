.PHONY: configure build run clean rebuild

BUILD_DIR := build
ARGS := input.txt

configure:
	cmake -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_CXX_COMPILER=clang++

build:
	cmake --build $(BUILD_DIR)

# run + build | make sure to include file as argument
runb: build
	./$(BUILD_DIR)/Interpreter.exe $(ARGS)

# make sure to include file as argument
run:
	./$(BUILD_DIR)/Interpreter.exe $(ARGS)

clean:
	cmake --build $(BUILD_DIR) --target clean

rebuild: clean build
