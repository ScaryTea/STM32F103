INCDIR = libs/incude/
OBJDIR = obj/
SRCDIR = libs/source/
LINKDIR = linker/

#INCDIR := "$(CURDIR)/$(INCDIR)"
#INCDIR := "$(CURDIR)/$(INCDIR)"
#OBJDIR := "$(CURDIR)/$(OBJDIR)"
#SRCDIR := "$(CURDIR)/$(SRCDIR)"
#LINKDIR := "$(CURDIR)/$(LINKDIR)"

STD ?= c99

FILENAME ?= main
OBJECTS = $(FILENAME).o startup_stm32f103xb.o system_stm32f1xx.o

COMP = arm-none-eabi-gcc -Wall --std=c99 -g3
COMP += -mthumb -mcpu=cortex-m3 -ffreestanding
COMP += -DDEBUG -DSTM32F103xB -c 

ELF = arm-none-eabi-gcc -Wall -Wl,-TSTM32F103XB_FLASH.ld
ELF += -std=$(STD) --specs=nosys.specs -mthumb
ELF += -mcpu=cortex-m3 $(OBJECTS) -o $(FILENAME).elf

BIN = arm-none-eabi-objcopy -O binary $(FILENAME).elf $(FILENAME).bin

#vpath %.h $(INCDIR)
#vpath %.o $(OBJDIR)
#vpath %.c $(SRCDIR)
#vpath %.s $(SRCDIR)
#vpath %.ld $(LINKDIR)

.PHONY: all flash clean

all: $(FILENAME).bin

$(FILENAME).bin: $(FILENAME).elf
	@$(BIN)

$(FILENAME).elf: $(OBJECTS)
	@$(ELF)

%.o : %.c
	@$(COMP) $^

%.o : %.s
	@$(COMP) $^

flash:
	@st-flash write $(FILENAME).bin 0x8000000

clean:
	@rm -f *.bin *.elf $(FILENAME).o
