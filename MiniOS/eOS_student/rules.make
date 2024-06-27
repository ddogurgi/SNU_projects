TARGET := $(shell pwd)/module.a

subdir_libs := $(shell pwd)/core/module.a $(shell pwd)/hal/linux/emulator/module.a \
				$(shell pwd)/hal/linux/module.a $(shell pwd)/user/module.a

subdir_targets:
	$(MAKE) -C $(shell pwd)/core $(shell pwd)/core/module.a
	$(MAKE) -C $(shell pwd)/hal/linux/emulator $(shell pwd)/hal/linux/emulator/module.a
	$(MAKE) -C $(shell pwd)/hal/linux $(shell pwd)/hal/linux/module.a
	$(MAKE) -C $(shell pwd)/user $(shell pwd)/user/module.a

# cleaning this directory
clean_all:
	$(MAKE) -C $(shell pwd)/core clean_
	$(MAKE) -C $(shell pwd)/hal/linux/emulator clean_
	$(MAKE) -C $(shell pwd)/hal/linux clean_
	$(MAKE) -C $(shell pwd)/user clean_
