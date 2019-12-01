PROJECT_ROOT = $(shell pwd)

DEBUG_CFLAGS := -g
RELEASE_CFLAGS := -O2 -DNDEBUG
CFLAGS := $(if $(findstring 1,$(NDEBUG)),$(RELEASE_CFLAGS),$(DEBUG_CFLAGS))
CPPFLAGS := -I$(PROJECT_ROOT)/src
LINKER :=

# Dependencies
## Mpack (msgpack)
CPPFLAGS := $(CPPFLAGS) -I$(PROJECT_ROOT)/deps -DMPACK_HAS_CONFIG

## libuv
CFLAGS := $(CFLAGS) $(shell pkg-config --cflags libuv)
LINKER := $(LINKER) $(shell pkg-config --libs libuv)

DEPS_OBJS := \
  deps/mpack/mpack.o

OBJS := \
  src/main.o \
  src/utils/memory_pool.o \
  src/nvim/api.o \
  src/nvim/funcs.o \
  src/nvim/storage.o \
  $(DEPS_OBJS)

TESTS_OBJS := \
  src/test.o \
  deps/unity/src/unity.c \
  $(filter-out src/main.o, $(OBJS))


.PHONY: all clean test
all: neobolt_server neobolt_tests

test: neobolt_tests
	@./neobolt_tests

clean:
	@echo Removing all translation units
	@rm -f $(shell find . -name '*.o')
	@echo Removing all depfiles
	@rm -f $(shell find . -name '*.d')
	@echo Removing linked binaries
	@rm -f neobolt_server neobolt_tests

compile_commands.json: clean
	@compiledb --full-path -o $@ make all

neobolt_server: $(OBJS)
	@echo CC    $@
	@$(CC) -o $@ $(LINKER) $(CFLAGS) $^

util_memcheck: neobolt_tests
	valgrind --leak-check=yes --track-origins=yes ./neobolt_tests

neobolt_tests: $(TESTS_OBJS)
	@echo CC    $@
	@echo 
	@$(CC) -o $@ $(LINKER) $(CFLAGS) $^

%.o: %.c
	@echo CC    $@
	@$(CC) -std=gnu18 -MD -c -o $@ $(CFLAGS) $(CPPFLAGS) $<

-include $(OBJS:.o=.d)
