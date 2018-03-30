# opencm3 lib config
LIBNAME         = opencm3_stm32f1
DEFS            += -DSTM32F1
LDLIBS		+=  $(ROOT)/src/u8g2/u8g2.a
CFLAGS += -I$(ROOT)/src/u8g2 -Wno-unused-parameter

FP_FLAGS        ?= -msoft-float
ARCH_FLAGS      = -mthumb -mcpu=cortex-m3 $(FP_FLAGS)

LDSCRIPT = linker/stm32f1.ld

reset:
	echo "reset" | nc -4 localhost 4444

