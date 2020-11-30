all:
	cd lib && $(MAKE)
	cd lib/usb && $(MAKE)
	cd lib/input && $(MAKE)
	cd lib/fs && $(MAKE)
	cd lib/fs/fatfs && $(MAKE)
	cd lib/sched && $(MAKE)
	cd lib/pegasos && $(MAKE)
	cd addon/SDCard && $(MAKE)
	cd sample/00-pegasos && $(MAKE)

	cd lib && rm *.o && rm *.d
	cd lib/usb && rm *.o && rm *.d
	cd lib/input && rm *.o && rm *.d
	cd lib/fs && rm *.o && rm *.d
	cd lib/fs/fatfs && rm *.o && rm *.d
	cd lib/sched && rm *.o && rm *.d
	cd lib/pegasos && rm *.o && rm *.d
	cd addon/SDCard && rm *.o && rm *.d
	cd sample/00-pegasos && rm *.o && rm *.d

clean:
	cd lib && rm *.o && rm *.d
	cd lib/usb && rm *.o && rm *.d
	cd lib/input && rm *.o && rm *.d
	cd lib/fs && rm *.o && rm *.d
	cd lib/fs/fatfs && rm *.o && rm *.d
	cd lib/sched && rm *.o && rm *.d
	cd lib/pegasos && rm *.o && rm *.d
	cd addon/SDCard && rm *.o && rm *.d
	cd sample/00-pegasos && rm *.o && rm *.d