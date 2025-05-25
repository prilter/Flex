# "RUN AS ROOT" ERROR
ifeq ($(shell id -u),0)
else
    $(error Run as root(use su))
endif

# VARIABLES
COMPILER  = gcc
TARGET    = bin/flex
SRC       = main.c 
LIBS      = -lcrypt 
CFLAGS    = 

# RULES FOR COMPILING
compile: $(TARGET)

debug: CFLAGS = -Wall -Wextra -Werror -g
debug: $(TARGET)

release: CFLAGS = -O3
release: $(TARGET)

# COMPILE
$(TARGET): $(SRC)
	@mkdir -p bin 
	$(COMPILER) $(CFLAGS) $< -o $@ $(LIBS)
	chown root:root $(TARGET)
	chmod 4111 $(TARGET)

# CLEAN
clean:
	rm -f $(TARGET)

# USING RULES
.PHONY: compile release debug clean
