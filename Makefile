# AVR cross-compiler toolchain is used here
HOST_CC = gcc
HOST_CFLAGS = -std=c99
CROSS_CC = avr-gcc
CROSS_CFLAGS = -Wall -Os -Iusbdrv -mmcu=$(PARTNO) -DF_CPU=$(CRYSTAL) -std=c99
CROSS_OBJCOPY = avr-objcopy
CROSS_OBJFLAGS = -j .text -j .data -O ihex
DUDE = avrdude
DUDEFLAGS = -p $(PARTNO) -c usbasp -v

# AVR microcontroller
PARTNO = attiny2313
CRYSTAL = 12000000

# Object files for the firmware (usbdrv/oddebug.o not strictly needed I think)
CROSS_OBJECTS = usbdrv/usbdrv.o usbdrv/oddebug.o usbdrv/usbdrvasm.o main.o

# Source files for host utils
HOST_OBJECTS = util/passwordSeed.o

# By default, build the firmware and command-line client, but do not flash
all: main.hex eeprom.bin

# With this, you can flash the firmware by just typing "make flash" on command-line
flash:	main.hex
	$(DUDE) $(DUDEFLAGS) -U flash:w:$<

eeprom:	eeprom.bin
	$(DUDE) $(DUDEFLAGS) -U eeprom:w:$<

# Housekeeping if you want it
clean:
	$(RM) *.o *.hex *.bin *.elf usbdrv/*.o util/*.o

# From .elf file to .hex
%.hex: %.elf
	$(CROSS_OBJCOPY) $(CROSS_OBJFLAGS) $< $@

# Main.elf requires additional objects to the firmware, not just main.o
main.elf: $(CROSS_OBJECTS)
	$(CROSS_CC) $(CROSS_CFLAGS) $(CROSS_OBJECTS) -o $@
	
passwordSeed.elf: $(HOST_OBJECTS)
	$(HOST_CC) $(HOST_CFLAGS) $(HOST_OBJECTS) -o $@

# Without this dependance, .o files will not be recompiled if you change 
# the config! I spent a few hours debugging because of this...
$(CROSS_OBJECTS): usbdrv/usbconfig.h

# From C source to .o object file with cross toolchain
%.o:	%.c
	$(CROSS_CC) $(CROSS_CFLAGS) -c $< -o $@
	
# From C source to .o object file with host toolchain
util/%.o: util/%.c
	$(HOST_CC) $(HOST_CFLAGS) -c $< -o $@

# From assembler source to .o object file with cross toolchain
%.o:	%.S
	$(CROSS_CC) $(CROSS_CFLAGS) -c $< -o $@

# Generate password seed
eeprom.bin: passwordSeed.elf
	./$< < /dev/random
