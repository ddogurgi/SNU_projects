TARGET := $(shell pwd)/module.a
SRC := comm.c common.c interrupt.c main.c scheduler.c sync.c task.c timer.c
OBJS := comm.o common.o interrupt.o main.o scheduler.o sync.o task.o timer.o

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

clean_:
	rm -rf $(TARGET) $(OBJS)
