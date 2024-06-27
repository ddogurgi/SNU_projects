TARGET := $(shell pwd)/module.a
SRC := context.c entry.S init.c interrupt_asm.S interrupt.c serial.c
OBJS := context.o entry.o init.o interrupt_asm.o interrupt.o serial.o

# compling and linking this directory
$(TARGET): banner $(OBJS)
ifneq ($(OBJS),)
	ar r $(TARGET) $(OBJS)
endif

banner:
	@echo ----------------------------------------------------
	@echo Compiling in $(shell pwd)
	@echo ----------------------------------------------------

%.o: %.c
	$(CC) -c -Os -Wall -D_KERNEL_ -I$(TOP_DIR) -o $@ $< 

%.o: %.S
	$(CC) -c -D_KERNEL_ -I$(TOP_DIR) -o $@ $<

clean_:
	rm -rf $(TARGET) $(OBJS)


clean: clean_all	
	rm -f $(TOP_DIR)/eos

eos: subdir_targets
	$(CC) -o $(TOP_DIR)/eos -Wl,--start-group $(subdir_libs) -Wl,--end-group -lrt
	@echo
	@echo Building EOS is complete. Type ./eos to run EOS.
