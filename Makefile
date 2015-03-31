OS := $(shell uname)
ARCH := $(shell uname -m)

LEAP_SDK := ../LeapDeveloperKit_2.2.4+26750_linux/LeapSDK

ifeq ($(OS), Linux)
  ifeq ($(ARCH), x86_64)
    LEAP_LIBRARY := $(LEAP_SDK)/lib/x64/libLeap.so -Wl,-rpath,$(LEAP_SDK)/lib/x64
  else
    LEAP_LIBRARY := $(LEAP_SDK)/lib/x86/libLeap.so -Wl,-rpath,$(LEAP_SDK)/lib/x86
  endif
else
  # OS X
  LEAP_LIBRARY := $(LEAP_SDK)/lib/libLeap.dylib
endif

leappoint: main.cpp
	$(CXX) -Wall -g -I$(LEAP_SDK)/include main.cpp -o leappoint $(LEAP_LIBRARY)
ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib $(LEAP_SDK)/lib/libLeap.dylib leappoint
endif

clean:
	rm -rf leappoint leappoint.dSYM
