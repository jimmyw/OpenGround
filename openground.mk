# opencm3 lib config
LIBNAME         = opencm3_stm32f0
DEFS            += -DSTM32F0

FP_FLAGS        ?= -msoft-float
ARCH_FLAGS      = -mthumb -mcpu=cortex-m0 $(FP_FLAGS)

LDSCRIPT = linker/stm32f072.ld


