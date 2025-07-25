BUILD_DIR = cmake_build

all: prebuild test check linux nucleo

prebuild:
	cd src/rte && python3 rte.py

linux:
	export GLOBAL_TARGET_PLATFORM=LINUX \
	&& mkdir -p $(BUILD_DIR)/LINUX      \
	&& cd $(BUILD_DIR)/LINUX            \
	&& cmake ../..                      \
	&& cmake --build .

nucleo:
	export GLOBAL_TARGET_PLATFORM=NUCLEO \
	&& mbed-tools deploy                 \
	&& mbed-tools compile -m NUCLEO_F446RE -t GCC_ARM

test:
	mkdir -p $(BUILD_DIR)/test \
	&& cd $(BUILD_DIR)/test    \
	&& cmake ../../tests       \
	&& cmake --build .

check:
	cd $(BUILD_DIR)/test \
	&& ./asb_test

run:
	cd $(BUILD_DIR)/LINUX \
	&& ./asb_linux

clean:
	rm -rf $(BUILD_DIR)
	rm -rf src/rte/generated
	