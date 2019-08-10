
DEVICE_CC        = $(SJCLANG)/bin/clang
DEVICE_CPPC      = $(SJCLANG)/bin/clang++
DEVICE_OBJDUMP   = $(SJCLANG)/bin/llvm-objdump
DEVICE_SIZEC     = $(SJCLANG)/bin/llvm-size
DEVICE_OBJCOPY   = $(SJCLANG)/bin/llvm-objcopy
DEVICE_NM        = $(SJCLANG)/bin/llvm-nm
DEVICE_AR        = $(SJCLANG)/bin/llvm-cov
DEVICE_ADDR2LINE = $(SJARMGCC)/bin/llvm-symbolizer
DEVICE_GDB       = gdb

LINKFLAGS = $(COMMON_FLAGS) -Wl,--gc-sections -Wl,-Map,"$(MAP)" \
            -stdlib=libc++ -lc++ -lc++abi

platform-flash:
	@echo
	@echo $(EXECUTABLE)
	@echo
	@./$(EXECUTABLE)
