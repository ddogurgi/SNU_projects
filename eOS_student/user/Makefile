TARGET := $(shell pwd)/module.a
SRC := work.c
OBJS := work.o

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
	$(CC) -c -Os -Wall -I$(TOP_DIR) -o $@ $< 

clean_:	
	rm -rf $(TARGET) $(OBJS)
