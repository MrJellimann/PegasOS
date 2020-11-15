#
# Makefile
#
# Here's how to use the makefile:
#     You're going to put the names of any .cpp files on the OBJS line, but with the .o extension
#     If the line gets super long, you break it with a '\' then continue on the next line.
#
# Uncomment the rest of the contents once you're ready to start having your stuff compiled.
# And please... once you have something useful, get rid of 'something.cpp' and 'something.h'.
# On that note, kernel.cpp does have an include for 'something.h' to prove that the convention works.

CIRCLEHOME = ../..

OBJS	= something.o shell.o

libpegasos.a: $(OBJS)
	@echo "  AR    $@"
	@rm -f $@
	@$(AR) cr $@ $(OBJS)

include $(CIRCLEHOME)/Rules.mk

-include $(DEPS)
