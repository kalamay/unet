all: bin

-include build/Custom.mk

NAME?= unet
OS:= $(shell uname -s)
ARCH?= $(shell uname -m)
ifeq ($(ARCH_TARGET),x86_32)
ARCHFLAGS?= -m32
else
ARCHFLAGS?= -m64
endif

CXXFLAGS:= -std=c++14 -MMD -fvisibility=hidden $(ARCHFLAGS) -fno-rtti -fno-exceptions

ifeq ($(BUILD),debug)
  CXXFLAGS+= -Wall -Wextra -Werror -g
else
  OPTFLAGS:= -O2
  ifeq ($(findstring BSD,$(OS)),)
    OPTFLAGS+= -flto
  endif
  CXXFLAGS+= $(OPTFLAGS)
endif

LDFLAGS:= $(OPTFLAGS) -lunwind# -static-libgcc -static-libstdc++

ifeq ($(OS),Linux)
  CXXFLAGS+= -D_POSIX_SOURCE -D_GNU_SOURCE
  LDFLAGS+= -ldl
endif

ifeq ($(OS),Darwin)
  SONAME:= lib$(NAME).dylib
  SOFLAGS:= -dynamiclib -current_version 1.0 -compatibility_version 1.0
else
  SONAME:= lib$(NAME).so
  SOFLAGS:= -shared
endif

BINSRC:= main.cc error.cc device.cc device_tun.cc eth.cc arp.cc fio/fio.cc
SOSRC:= 

BIN:= build/bin/$(NAME)
BINOBJ:= $(BINSRC:%.cc=build/tmp/%.o)

SO:= build/lib/$(SONAME)
SOOBJ:= $(SOSRC:%.cc=build/tmp/%.o)

DEP:= $(BINOBJ:%.o=%.d) $(SOOBJ:%.o=%.d)

bin: $(BIN)

$(BIN): $(BINOBJ) | build/bin
	$(CXX) $^ -o $@ $(LDFLAGS) $(CXXFLAGS)

$(SO): $(SOOBJ) | build/lib
	$(CXX) $^ -o $@ $(LDFLAGS) $(SOFLAGS)

build/tmp/%.o: src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $<	-o $@	$(CXXFLAGS)

build/bin build/lib:
	mkdir $@

clean:
	rm -rf build/tmp build/bin build/lib

.PHONY: all _all run clean

-include $(DEP)
