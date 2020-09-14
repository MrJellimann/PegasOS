# PegasOS
The public open-source repository for PegasOS, a 64-bit ARMv8 Operating System, primarily for the Raspberry Pi

The documentation for this OS is listed [here](https://github.com/MrJellimann/PegasOSDocumentation)

The source code for this OS is on this repository. (You are here!)

# Setup

## Requirements
* Image Flasher
* Raspbian OS
* Cross-Compiler
* Ubuntu or Windows Subsystem for Linux

## Installation
1. First, download an image flasher and verison of Raspbian OS. I used BalenaEtcher for this setup and downloaded the Raspbian OS Lite.
2. After downloading these, I flashed Raspian OS on my MicroSD card using the BalenaEtcher.
3. After flashing the SD card, you will need to setup the cross compiler. For more information on how to do this, see the section about Compiling Circle below. *YOU SHOULD SET UP YOUR CROSS COMPILER COMPLETELY BEFORE COMPILING THE OS*
4. Now navigate into the project directory, into the Sample folder and into 00-pegasos. Type `make` in the console to compile PegasOS alongside the Circle Library. This will produce a kernel image file.
5. Navigate into the SD card that you flashed Raspbian to, and delete the kernel8.img file there. Replace it with the compiled PeagsOS kernel image.
6. Insert the SD card into your Raspberry Pi and boot!

## Compiling Circle
To cross-compile Circle on Linux, do the following:

Here, we will be doing this from Windows 10 running WSL on Debian (the steps are *roughly* the same for Ubuntu).

=== Part 1 - Installing the Cross-Compiler for AArch64 ===

Step 1.

	Go to
		https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads
	And download the appropriate (aarch64-none-elf) compiler FOR YOUR CPU
		Intel CPU running Linux -> gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz
	If you are compiling this on an ARM cpu (such as a windows tablet), try this one instead
		ARM64 CPU running Linux -> gcc-arm-9.2-2019.12-aarch64-aarch64-none-elf.tar.xz

Step 2.

	Extract the contents to a new folder.

Step 3.

	Open your WSL Distro (in my case, Debian).

Step 4.

	Create a directory called /opt
	(Note: the directory name doesn't matter, but being consistent does)

Step 5.

	Open the folder in windows with `explorer.exe .`

Step 6.

	Open the folder you extracted the compiler to.

Step 7.

	Copy the folders within the compiler's folder into /opt.
	These folders are called 'aarch64-none-elf', 'bin', 'include', 'lib', 'libexec', 'share'

Step 8.

	After the folders have copied over, open the `.profile` file for your user in your text editor.
  If you are using Ubuntu, repeat steps 8 and 9 for your `.profile.save` file as well.
	Sample: `nano ~/.profile`

Step 9.

	At the bottom of the file, add a line that says:
		`export PATH="$HOME/opt/bin:$PATH"`
		(Note: you may substitute this with a custom directory if you did that for Step 4)

Step 10.

	Restart your computer for the path changes to take effect.

Step 11.

	Reopen your WSL Distro.

Step 12.

	To confirm the path variable works correctly, type:
		`aarch64-none-elf-gcc -v`
		or
		`aarch64-none-elf-gcc --version`

Step 13.

	If you see a copyright message, you've done it!

=== Part 2 - Compiling the Libraries ===

Step 14.

	Clone the Circle project
		https://github.com/rsta2/circle

Step 15.

	Move into the /circle/lib directory of the repository

Step 16.

	`make` the /lib folder

Step 17.

	Move into the /circle/lib/usb directory

Step 18.

	`make` the /lib/usb folder

Step 19.

	Move into the /circle/lib/input directory

Step 20.

	`make` the /lib/input folder

Step 21.

	Move into the /circle/lib/fs directory

Step 22.

	`make` the /lib/fs folder

Step 23.

	Move into the /circle/lib/fs/fat directory

Step 24.

	`make` the /lib/fs/fat folder

Step 25.

	Move into the /circle/lib/sched directory

Step 26.

	`make` the /lib/sched folder

Step 27.

	You've compiled the Circle library! (at least the important stuff)

=== Part 3 - Compiling the Sample Kernel ===

Step 28.

	Move into the desired sample folder. In this case, the usbkeyboard folder
	/circle/sample/08-usbkeyboard

Step 29.

	`make` the desired folder

Step 30.

	If you see a 'kernel8-rpi4.img' file, you've done it!

=== Part 4 - Move files onto RPi Micro SD and Boot ===

Step 31.

	Insert your Micro SD card into your Micro SD card reader/adapter and into your PC

Step 32.

	Make sure that your Micro SD card is formatted for FAT file system

Step 33.

	Copy the files from /circle/boot onto your Micro SD card

Step 34.

	Copy the kernel image from Step 29 onto your Micro SD card

Step 35.

	Make sure that the 'config.txt' file is present on the Micro SD card for 64-bit boot

Step 36.

	Now remove your Micro SD card and reinsert it into your RPi

Step 37.

	Plug in your Pi and boot

Step 38.

	If you see text about Circle, you've done it!

PegasOS and Circle
======

For completeness and inclusiveness, the Circle Library Readme is included below from Release 42.

Circle
======

> If you read this file in an editor you should switch line wrapping on.

Overview
--------

Circle is a C++ bare metal programming environment for the Raspberry Pi. It should be usable on all existing models (tested on model A+, B, B+, on Raspberry Pi 2, 3, 4 and on Raspberry Pi Zero). It provides several ready-tested [C++ classes](doc/classes.txt) and [add-on libraries](addon/README), which can be used to control different hardware features of the Raspberry Pi. Together with Circle there are delivered several [sample programs](sample/README), which demonstrate the use of its classes. Circle can be used to create 32-bit or 64-bit bare metal applications.

Circle includes bigger (optional) third-party C-libraries for specific purposes in addon/ now. This is the reason why GitHub rates the project as a C-language-project. The main Circle libraries are written in C++ using classes instead. That's why it is named a C++ programming environment.

The 42nd Step
-------------

This release adds **Wireless LAN access** support in [addon/wlan](addon/wlan) to Circle. Please read the [README file](addon/wlan/sample/README) of the sample program for details! The WLAN support in Circle is still experimental.

To allow parallel access to WLAN and SD card, a new **SDHOST driver** for SD card access on Raspberry Pi 1-3 and Zero has been added. You can return to the previous EMMC interface in case of problems (e.g. if using QEMU) or for real-time applications by adding `DEFINE += -DNO_SDHOST` to *Config.mk*. WLAN access is not possible then. On Raspberry Pi 4 the **EMMC2 interface** is used for SD card access now.

Features
--------

Circle supports the following features:

| Group                 | Features                                            |
|-----------------------|-----------------------------------------------------|
| C++ build environment | AArch32 and AArch64 support                         |
|                       | Basic library functions (e.g. new and delete)       |
|                       | Enables all CPU caches using the MMU                |
|                       | Interrupt support (IRQ and FIQ)                     |
|                       | Multi-core support (Raspberry Pi 2, 3 and 4)        |
|                       | Cooperative non-preemtive scheduler                 |
|                       | CPU clock rate management                           |
|                       |                                                     |
| Debug support         | Kernel logging to screen, UART and/or syslog server |
|                       | C-assertions with stack trace                       |
|                       | Hardware exception handler with stack trace         |
|                       | GDB support using rpi_stub (Raspberry Pi 2 and 3)   |
|                       | Serial bootloader (by David Welch) included         |
|                       | Software profiling support (single-core)            |
|                       | QEMU support                                        |
|                       |                                                     |
| SoC devices           | GPIO pins (with interrupt, Act LED) and clocks      |
|                       | Frame buffer (screen driver with escape sequences)  |
|                       | UART(s) (Polling and interrupt driver)              |
|                       | System timer (with kernel timers)                   |
|                       | Platform DMA controller                             |
|                       | EMMC SD card interface driver                       |
|                       | PWM output (2 channels)                             |
|                       | PWM sound output (on headphone jack)                |
|                       | I2C master(s) and slave                             |
|                       | SPI0 master (Polling and DMA driver)                |
|                       | SPI1 auxiliary master (Polling)                     |
|                       | SPI3-6 masters of Raspberry Pi 4 (Polling)          |
|                       | I2S sound output                                    |
|                       | Hardware random number generator                    |
|                       | Official Raspberry Pi touch screen                  |
|                       | VCHIQ interface and audio service drivers           |
|                       | BCM54213PE Gigabit Ethernet NIC of Raspberry Pi 4   |
|                       |                                                     |
| USB                   | Host controller interface (HCI) drivers             |
|                       | Standard hub driver (USB 2.0 only)                  |
|                       | HID class device drivers (keyboard, mouse, gamepad) |
|                       | Driver for on-board Ethernet device (SMSC951x)      |
|                       | Driver for on-board Ethernet device (LAN7800)       |
|                       | Driver for USB mass storage devices (bulk only)     |
|                       | Audio class MIDI input support                      |
|                       | Printer driver                                      |
|                       |                                                     |
| File systems          | Internal FAT driver (limited function)              |
|                       | FatFs driver (full function, by ChaN)               |
|                       |                                                     |
| TCP/IP networking     | Protocols: ARP, IP, ICMP, UDP, TCP                  |
|                       | Clients: DHCP, DNS, NTP, HTTP, Syslog, MQTT         |
|                       | Servers: HTTP, TFTP                                 |
|                       | BSD-like C++ socket API                             |
|                       |                                                     |
| Graphics              | OpenGL ES 1.1 and 2.0, OpenVG 1.1, EGL 1.4          |
|                       | (not on Raspberry Pi 4)                             |
|                       | uGUI (by Achim Doebler)                             |
|                       | LittlevGL GUI library (by Gabor Kiss-Vamosi)        |

Building
--------

> For building 64-bit applications (AArch64) see the next section.

Building is normally done on PC Linux. If building for the Raspberry Pi 1 you need a [toolchain](http://elinux.org/Rpi_Software#ARM) for the ARM1176JZF core (with EABI support). For Raspberry Pi 2/3/4 you need a toolchain with Cortex-A7/-A53/-A72 support. A toolchain, which works for all of these, can be downloaded [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads). Circle has been tested with the version *9.2-2019.12* (gcc-arm-9.2-2019.12-x86_64-arm-none-eabi.tar.xz) from this website.

First edit the file *Rules.mk* and set the Raspberry Pi version (*RASPPI*, 1, 2, 3 or 4) and the *PREFIX* of your toolchain commands. Alternatively you can create a *Config.mk* file (which is ignored by git) and set the Raspberry Pi version and the *PREFIX* variable to the prefix of your compiler like this (don't forget the dash at the end):

```
RASPPI = 1
PREFIX = arm-none-eabi-
```

The following table gives support for selecting the right *RASPPI* value:

| RASPPI | Target         | Models                   | Optimized for |
| ------ | -------------- | ------------------------ | ------------- |
|      1 | kernel.img     | A, B, A+, B+, Zero, (CM) | ARM1176JZF-S  |
|      2 | kernel7.img    | 2, 3, (CM3)              | Cortex-A7     |
|      3 | kernel8-32.img | 3, (CM3)                 | Cortex-A53    |
|      4 | kernel7l.img   | 4                        | Cortex-A72    |

For a binary distribution you should do one build with *RASPPI = 1*, one with *RASPPI = 2* and one build with *RASPPI = 4* and include the created files *kernel.img*, *kernel7.img* and *kernel7l.img*. Optionally you can do a build with *RASPPI = 3* and add the created file *kernel8-32.img* to provide an optimized version for the Raspberry Pi 3.

Then go to the build root of Circle and do:

```
./makeall clean
./makeall
```

By default only the latest sample (with the highest number) is build. The ready build *kernel.img* file should be in its subdirectory of sample/. If you want to build another sample after `makeall` go to its subdirectory and do `make`.

You can also build Circle on the Raspberry Pi itself (set `PREFIX =` (empty)) on Raspbian but you need some method to put the *kernel.img* file onto the SD(HC) card. With an external USB card reader on model B+ or Raspberry Pi 2/3/4 model B (4 USB ports) this should be no problem.

Building Circle from a non-Linux host is possible too. Maybe you have to adapt the shell scripts in this case. You need a cross compiler targetting (for example) *arm-none-eabi*. OSDev.org has an [excellent document on the subject](http://wiki.osdev.org/GCC_Cross-Compiler) that you can follow if you have no idea of what a cross compiler is.

AArch64
-------

Circle supports building 64-bit applications, which can be run on the Raspberry Pi 3 or 4. There are also Raspberry Pi 2 versions, which are based on the BCM2837 SoC. These Raspberry Pi versions can be used too.

The recommended toolchain to build 64-bit applications with Circle can be downloaded [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads). Circle has been tested with the version *9.2-2019.12* (gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz) from this website.

First edit the file *Rules.mk* and set the Raspberry Pi architecture (*AARCH*, 32 or 64) and the *PREFIX64* of your toolchain commands. The *RASPPI* variable has to be set to 3 or 4 for `AARCH = 64`. Alternatively you can create a *Config.mk* file (which is ignored by git) and set the Raspberry Pi architecture and the *PREFIX64* variable to the prefix of your compiler like this (don't forget the dash at the end):

```
AARCH = 64
RASPPI = 3
PREFIX64 = aarch64-none-elf-
```

Then go to the build root of Circle and do:

```
./makeall clean
./makeall
```

By default only the latest sample (with the highest number) is build. The ready build *kernel8.img* or *kernel8-rpi4.img* file should be in its subdirectory of sample/. If you want to build another sample after `makeall` go to its subdirectory and do `make`.

Installation
------------

Copy the Raspberry Pi firmware (from boot/ directory, do *make* there to get them) files along with the *kernel.img* (from sample/ subdirectory) to a SD(HC) card with FAT file system. Put the SD(HC) card into the Raspberry Pi.

The *config.txt* file, provided in the boot/ directory, is only needed to enable 64-bit mode and has to be copied on the SD card in this case. It must not be on the SD card otherwise!

FIQ support for AArch64 on the Raspberry Pi 4 requires an additional file *armstub8-rpi4.bin* on the SD card. Please see [boot/README](boot/README) for information on how to build this file.

Directories
-----------

* include: The common header files, most class headers are in the include/circle/ subdirectory.
* lib: The Circle class implementation and support files (other libraries are in subdirectories of lib/).
* sample: Several sample applications using Circle in different subdirectories. The main function is implemented in the CKernel class.
* addon: Contains contributed libraries and samples (has to be build manually).
* app: Place your own applications here. If you have own libraries put them into app/lib/.
* boot: Do *make* in this directory to get the Raspberry Pi firmware files required to boot.
* doc: Additional documentation files.
* tools: Some tools for using Circle more comfortable (e.g. a serial bootloader).

Classes
-------

The available Circle classes are listed in the file [doc/classes.txt](doc/classes.txt). If you have Doxygen installed on your computer you can build a [class documentation](doc/html/index.html) in doc/html/ using:

`./makedoc`

At the moment there are only a few classes described in detail for Doxygen.

Additional Topics
-----------------

* [Standard library support](doc/stdlib-support.txt)
* [Dynamic memory management and the "new" operator](doc/new-operator.txt)
* [Serial bootloader support](doc/bootloader.txt)
* [Multi-core support](doc/multicore.txt)
* [Debugging support](doc/debug.txt)
* [QEMU support](doc/qemu.txt)
* [Eclipse IDE support](doc/eclipse-support.txt)
* [About real-time applications](doc/realtime.txt)
* [cmdline.txt options](doc/cmdline.txt)
* [Screen escape sequences](doc/screen.txt)
* [Keyboard escape sequences](doc/keyboard.txt)
* [Memory layout](doc/memorymap.txt)
* [Known issues](doc/issues.txt)

Trademarks
----------

Raspberry Pi is a trademark of the Raspberry Pi Foundation.

Linux is a trademark of Linus Torvalds.

PS3 and PS4 are registered trademarks of Sony Computer Entertainment Inc.

Xbox 360 and Xbox One are trademarks of the Microsoft group of companies.

Nintendo Switch is a trademark of Nintendo.

Khronos and OpenVG are trademarks of The Khronos Group Inc.

OpenGL ES is a trademark of Silicon Graphics Inc.
