# AVR cross-compiler toolchain is used here
HOST_CC = gcc
HOST_CFLAGS = -Icommon -Iutil -std=c99 -g
CROSS_CC = avr-gcc
CROSS_CFLAGS = -Wall -Os -Iusbdrv -Iutil -Icommon -mmcu=$(PARTNO) -DF_CPU=$(CRYSTAL) -std=gnu99
CROSS_OBJCOPY = avr-objcopy
CROSS_OBJFLAGS = -j .text -j .data -O ihex
DUDE = avrdude
DUDEFLAGS = -p $(PARTNO) -c usbasp -v

# AVR microcontroller
PARTNO = attiny4313
CRYSTAL = 12000000

# Object files for the firmware (usbdrv/oddebug.o not strictly needed I think)
CROSS_OBJECTS = usbdrv/usbdrv.cross.o \
	usbdrv/oddebug.cross.o \
	usbdrv/usbdrvasm.cross.o \
	button.cross.o \
	display.cross.o \
	keyboard.cross.o \
	main.cross.o \
	usb.cross.o \
	common/random.cross.o \
	common/conversionTable.cross.o

# Source files for host PasswordSeed util
SEED_HOST_OBJECTS = util/passwordSeed.host.o

# Source files for host PasswordDecrypt util
DECRYPT_HOST_OBJECTS = util/passwordDecrypt.host.o common/conversionTable.host.o common/random.host.o

# By default, build the firmware and command-line client, but do not flash
all: main.hex eeprom.bin

# With this, you can flash the firmware by just typing "make flash" on command-line
flash:	main.hex
	$(DUDE) $(DUDEFLAGS) -U flash:w:$<

eeprom:	eeprom.bin
	$(DUDE) $(DUDEFLAGS) -U eeprom:w:$<

# Housekeeping if you want it
clean:
	$(RM) *.o *.hex *.bin *.elf usbdrv/*.o util/*.o common/*.o

# From .elf file to .hex
%.hex: %.elf
	$(CROSS_OBJCOPY) $(CROSS_OBJFLAGS) $< $@

# Main.elf requires additional objects to the firmware, not just main.o
main.elf: $(CROSS_OBJECTS)
	$(CROSS_CC) $(CROSS_CFLAGS) $(CROSS_OBJECTS) -o $@
	
passwordSeed.elf: $(SEED_HOST_OBJECTS)
	$(HOST_CC) $(HOST_CFLAGS) $(SEED_HOST_OBJECTS) -o $@
	
passwordDecrypt.elf: $(DECRYPT_HOST_OBJECTS)
	$(HOST_CC) $(HOST_CFLAGS) $(DECRYPT_HOST_OBJECTS) -o $@

# Without this dependance, .o files will not be recompiled if you change 
# the config! I spent a few hours debugging because of this...
$(CROSS_OBJECTS): usbdrv/usbconfig.h

# From C source to .o object file with cross toolchain
%.cross.o:	%.c
	$(CROSS_CC) $(CROSS_CFLAGS) -c $< -o $@
	
# From C source to .o object file with host toolchain
%.host.o: %.c
	$(HOST_CC) $(HOST_CFLAGS) -c $< -o $@

# From assembler source to .o object file with cross toolchain
%.cross.o:	%.S
	$(CROSS_CC) $(CROSS_CFLAGS) -c $< -o $@

# Generate password seed
eeprom.bin: passwordSeed.elf passwordDecrypt.elf
	./$< < /dev/random
