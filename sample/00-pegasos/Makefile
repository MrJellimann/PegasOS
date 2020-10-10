#
# Makefile
#

CIRCLEHOME = ../..

OBJS	= main.o kernel.o

LIBS	= $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/fs/fatfs/libfatfs.a\
	  $(CIRCLEHOME)/lib/libcircle.a\
	  $(CIRCLEHOME)/addon/SDCard/libsdcard.a

include ../Rules.mk

-include $(DEPS)
