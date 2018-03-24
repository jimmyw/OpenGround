# opencm3 lib config
LIBNAME         = opencm3_stm32f1
DEFS            += -DSTM32F1

FP_FLAGS        ?= -msoft-float
ARCH_FLAGS      = -mthumb -mcpu=cortex-m3 $(FP_FLAGS)

LDSCRIPT = linker/stm32f1.ld


