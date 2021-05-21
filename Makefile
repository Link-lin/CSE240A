
default: 
	@echo 'choose a target'

rundocker:
	$(MAKE) build -C docker/
	$(MAKE) run -C docker/


.PHONY: all clean 