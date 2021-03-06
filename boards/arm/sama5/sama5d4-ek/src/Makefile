############################################################################
# boards/arm/sama5/sama5d4-ek/src/Makefile
#
#   Copyright (C) 2014 Gregory Nutt. All rights reserved.
#   Author: Gregory Nutt <gnutt@nuttx.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name NuttX nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
############################################################################

include $(TOPDIR)/Make.defs

ASRCS =
CSRCS = sam_boot.c

ifeq ($(CONFIG_SAMA5_SPI0),y)
CSRCS += sam_spi.c
else
ifeq ($(CONFIG_SAMA5_SPI1),y)
CSRCS += sam_spi.c
endif
endif

ifeq ($(CONFIG_SAMA5_DDRCS),y)
CSRCS += sam_sdram.c
endif

ifeq ($(CONFIG_SAMA5_EBICS3_NAND),y)
CSRCS += sam_nandflash.c
endif

ifeq ($(CONFIG_SAMA5D4EK_DRAM_MAIN),y)
CSRCS += dram_main.c
endif

ifeq ($(CONFIG_SAMA5D4EK_AT25_MAIN),y)
CSRCS += at25_main.c
endif

ifeq ($(CONFIG_MTD_AT25),y)
ifeq ($(CONFIG_SAMA5_SPI0),y)
CSRCS += sam_at25.c
endif
endif

ifeq ($(CONFIG_FS_AUTOMOUNTER),y)
CSRCS += sam_automount.c
endif

ifeq ($(CONFIG_AUDIO_WM8904),y)
ifeq ($(CONFIG_SAMA5_TWI0),y)
ifeq ($(CONFIG_SAMA5_SSC0),y)
CSRCS += sam_wm8904.c
endif
endif
endif

ifeq ($(CONFIG_SAMA5_TWI0),y)
CSRCS += sam_pmic.c
endif

ifeq ($(CONFIG_AUDIO_NULL),y)
CSRCS += sam_audio_null.c
endif

ifeq ($(CONFIG_SAMA5_HSMCI0),y)
CSRCS += sam_hsmci.c
else
ifeq ($(CONFIG_SAMA5_HSMCI1),y)
CSRCS += sam_hsmci.c
endif
endif

ifeq ($(CONFIG_SAMA5_UHPHS),y)
CSRCS += sam_usb.c
else
ifeq ($(CONFIG_SAMA5_UDPHS),y)
CSRCS += sam_usb.c
endif
endif

ifeq ($(CONFIG_SAMA5_EMACB),y)
CSRCS += sam_ethernet.c
endif

ifeq ($(CONFIG_INPUT_MXT),y)
CSRCS += sam_maxtouch.c
endif

ifeq ($(CONFIG_LIB_BOARDCTL),y)
CSRCS += sam_appinit.c sam_bringup.c
else
ifeq ($(CONFIG_BOARD_LATE_INITIALIZE),y)
CSRCS += sam_bringup.c
endif
endif

ifeq ($(CONFIG_ADC),y)
CSRCS += sam_adc.c
endif

ifeq ($(CONFIG_PWM),y)
CSRCS += sam_pwm.c
endif

ifeq ($(CONFIG_USBMSC),y)
CSRCS += sam_usbmsc.c
endif

ifeq ($(CONFIG_ARCH_FPU),y)
CSRCS += sam_ostest.c
endif

ifeq ($(CONFIG_ARCH_LEDS),y)
CSRCS += sam_autoleds.c
else
CSRCS += sam_userleds.c
endif

ifeq ($(CONFIG_ARCH_BUTTONS),y)
CSRCS += sam_buttons.c
endif

include $(TOPDIR)/boards/Board.mk
