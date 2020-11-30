//
// kernel.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2017  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// this is the basic circle kernel information for the OS to boot 
#ifndef _kernel_h
#define _kernel_h

#include <circle/memory.h>
#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/usb/usbhcidevice.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>
#include <circle/types.h>

#define MAX_INPUT_LENGTH 256
#define MAX_DIRECTORY_LENGTH 1024

enum TShutdownMode
{
	ShutdownNone,
	ShutdownHalt,
	ShutdownReboot
};

class CKernel
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);

	TShutdownMode Run (void);

	static void SystemReboot();
	static void SystemOff();

	static CMemorySystem *GetKernelMemory();
	static CActLED *GetKernelActLED();
	static CDeviceNameService *GetKernelDNS();
	static CScreenDevice *GetKernelScreenDevice();
	static CSerialDevice *GetKernelSerialDevice();
	static CExceptionHandler *GetKernelExceptionHandler();
	static CInterruptSystem *GetKernelInterruptSystem();
	static CTimer *GetKernelTimer();
	static CLogger *GetKernelLogger();

private:
	static void KeyPressedHandler (const char *pString);
	static void ShutdownHandler (void);
	static void commenceLogin();

	static void KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6]);
	
private:
	// do not change this order
	CMemorySystem			m_Memory;
	CActLED					m_ActLED;
	CKernelOptions			m_Options;
	CDeviceNameService		m_DeviceNameService;
	CScreenDevice			m_Screen;
	CSerialDevice			m_Serial;
	CExceptionHandler		m_ExceptionHandler;
	CInterruptSystem		m_Interrupt;
	CTimer					m_Timer;
	CLogger					m_Logger;
<<<<<<< HEAD
=======
	CScheduler				m_Scheduler;
	CSynchronizationEvent	m_Event;
>>>>>>> parent of 0fe681b... Merge branch 'master' into revzet_branch
	CUSBHCIDevice			m_USBHCI;
	CEMMCDevice				m_EMMC;
	volatile TShutdownMode 	m_ShutdownMode;

	static CKernel *s_pThis;
	
	FATFS			m_FileSystem;
};

#endif