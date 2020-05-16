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
1. First, download an image flasher program and Raspbian OS. I used BalenaEtcher for this setup and downloaded the Raspbian OS Lite.
2. After downloading these, I flashed Raspbian OS on my MicroSD card using the Image Flasher program.
3. After flashing the SD card, you will need to setup the cross compiler. To do this, download the cross compiler I linked below.
https://drive.google.com/open?id=160vJBxZeM5sEN2byHh4-GKmf33bKgtXV
4. Once downloaded, you need to find a place to put it. I create a folder in the home directory called cross and put all of the folders in there.
5. After finding a directory for your cross compiler, you need to add the directory to the PATH environment variable. To do this, you need to type `nano ~/.profile` in your console and at the bottom add this line
```export PATH="(insert cross compiler's directory here)/bin:$PATH"```
Restart your computer in order for it to take effect.
6. Now go into your project directory and type make in the console to compile everything. This should produce a kernel8.img file. Finally, put this file in your microSD card to replace the old kernel8.img and you should be done.

If you are confused by these instructions, there is a video tutorial below that goes over the exact same steps.

## Video Tutorial

https://drive.google.com/open?id=1g3XmTwJFjrkqZVOPNT4OZAt8s3euBrZV

## January 27th, 2020

As the OS gets more filled out and designs/documents are created, they will go in the link above.
