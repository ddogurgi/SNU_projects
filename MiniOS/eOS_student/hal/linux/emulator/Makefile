TARGET := $(shell pwd)/module.a
SRC := intr.c timer.c vector.c
OBJS := intr.o timer.o vector.o

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
