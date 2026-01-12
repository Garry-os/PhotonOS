BUILD_DIR = build

.PHONY: all osloader clean

all: osloader

osloader: 
	@ $(MAKE) -C osloader

