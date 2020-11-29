//
// kernel.cpp
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
// this is the inital kernel to get the keyboard running. PegasOS main functions should go here
#include "kernel.h"
#include "screentask.h"
#include "primetask.h"
#include "ledtask.h"
#include <circle/usb/usbkeyboard.h>
#include <circle/string.h>
#include <circle/util.h>
#include <assert.h>
#include <pegasos/something.h>
#include <pegasos/shell.h>


#define DRIVE		"SD:"
//#define DRIVE		"USB:"

#define FILENAME	"/circle.txt"

static const char FromKernel[] = "kernel";
// int stringLen, globalIndex=0;
// char inputByUser[200], message[MAX_INPUT_LENGTH]="Command was found!"; /////////
// char directory[MAX_DIRECTORY_LENGTH];
// char mainCommandName[MAX_DIRECTORY_LENGTH];
// char commandParameter[MAX_INPUT_LENGTH];
// char userName[MAX_INPUT_LENGTH]="GiancarloGuillen";
// char helloMessagePartOne[MAX_INPUT_LENGTH]="Well hello there ";
// char helloMessagePartTwo[MAX_INPUT_LENGTH]=", and welcome to PegasOS!";
// char helpMessage1[]="This is a list of the Commands for PegasOS:\n\tbackgroundpalette\n\tcd\n\tclear\n\tconcat\n\tcopy\n\tcreatedir";
// char helpMessage2[]="\n\tcreatefile\n\tcurrentdir\n\tdelete\n\tdeletedir\n\techo\n\tfilespace\n\tfind\n\thead\n\thello\n\thelp\n\tlogin\n\tmount\n\tmove\n\tpower\n\tsysteminfo\n\ttail";
// char helpMessage3[]="\n\tmount\n\tmove\n\tpower\n\tsysteminfo\n\ttail\n\ttasklist\n\ttermiantetask\n\ttextpalette\n\tuninstall\n";
// FIL NewFIle;

CKernel *CKernel::s_pThis = 0;
PShell *PegasosShell = 0;

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_USBHCI (&m_Interrupt, &m_Timer),
	m_EMMC (&m_Interrupt, &m_Timer, &m_ActLED),
	m_ShutdownMode (ShutdownNone)
	//m_Shell (20)
{
	s_pThis = this;

	PegasosShell = new PShell();
	PegasosShell->AssignKernel(s_pThis);

	m_ActLED.Blink (5);	// show we are alive
}

CKernel::~CKernel (void)
{
	s_pThis = 0;
}

// basic keyboard setup
boolean CKernel::Initialize (void)
{
	// Proving linking bullshit
	//int rando = something;
	awful_funct();

	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}

	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (pTarget);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_USBHCI.Initialize ();
	}
	
	if (bOK)
	{
		bOK = m_EMMC.Initialize ();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);


	// Mount file system
	if (f_mount (&m_FileSystem, DRIVE, 1) != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot mount drive: %s", DRIVE);
	}

	if (m_FileSystem.fs_type == FS_EXFAT)
	{
		m_Logger.Write (FromKernel, LogNotice, "\t\tExFAT <ACTIVE>");
	}

	if (m_FileSystem.fs_type == FS_FAT12)
	{
		m_Logger.Write (FromKernel, LogNotice, "\t\tFAT12 <ACTIVE>");
	}

	if (m_FileSystem.fs_type == FS_FAT16)
	{
		m_Logger.Write (FromKernel, LogNotice, "\t\tFAT16 <ACTIVE>");
	}

	if (m_FileSystem.fs_type == FS_FAT32)
	{
		m_Logger.Write (FromKernel, LogNotice, "\t\tFAT32 <ACTIVE>");
	}
	// Show contents of root directory
	DIR Directory;
	FILINFO FileInfo;
	FRESULT Result = f_findfirst (&Directory, &FileInfo, DRIVE "/", "*");
	for (unsigned i = 0; Result == FR_OK && FileInfo.fname[0]; i++)
	{
		if (!(FileInfo.fattrib & (AM_HID | AM_SYS)))
		{
			CString FileName;
			FileName.Format ("%-19s", FileInfo.fname);

			//m_Screen.Write ((const char *) FileName, FileName.GetLength ());

			if (i % 4 == 3)
			{
				//m_Screen.Write ("\n", 1);
			}
		}

		Result = f_findnext (&Directory, &FileInfo);
	}
	//m_Screen.Write ("\n", 1);

	
	// Create file and write to it
	FIL File;
	Result = f_open (&File, DRIVE FILENAME, FA_WRITE | FA_CREATE_ALWAYS);
	if (Result != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot create file: %s", FILENAME);
	}

	
	for (unsigned nLine = 1; nLine <= 5; nLine++)
	{
		CString Msg;
		Msg.Format ("Hello File! (Line %u)\n", nLine);

		unsigned nBytesWritten;
		if (   f_write (&File, (const char *) Msg, Msg.GetLength (), &nBytesWritten) != FR_OK
		    || nBytesWritten != Msg.GetLength ())
		{
			m_Logger.Write (FromKernel, LogError, "Write error");
			break;
		}
	}

	
	if (f_close (&File) != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot close file");
	}

	// Reopen file, read it and display its contents
	Result = f_open (&File, DRIVE FILENAME, FA_READ | FA_OPEN_EXISTING);
	if (Result != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot open file: %s", FILENAME);
	}
	
	char Buffer[100];
	unsigned nBytesRead;
	while ((Result = f_read (&File, Buffer, sizeof Buffer, &nBytesRead)) == FR_OK)
	{
		if (nBytesRead > 0)
		{
			m_Screen.Write (Buffer, nBytesRead);
		}

		if (nBytesRead < sizeof Buffer)		// EOF?
		{
			break;
		}
	}

	
	if (Result != FR_OK)
	{
		m_Logger.Write (FromKernel, LogError, "Read error");
	}
	
	if (f_close (&File) != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot close file");
	}


	// Reopen file, read it and display its contents
	Result = f_open (&File, DRIVE FILENAME, FA_READ | FA_OPEN_EXISTING);
	if (Result != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot open file: %s", FILENAME);
	}
	commenceLogin();

	CUSBKeyboardDevice *pKeyboard = (CUSBKeyboardDevice *) m_DeviceNameService.GetDevice ("ukbd1", FALSE);
	if (pKeyboard == 0)
	{
		m_Logger.Write (FromKernel, LogError, "Keyboard not found");

		return ShutdownHalt;
	}
	
#if 1	// set to 0 to test raw mode
	pKeyboard->RegisterShutdownHandler (ShutdownHandler);
	pKeyboard->RegisterKeyPressedHandler (KeyPressedHandler);
#else
	pKeyboard->RegisterKeyStatusHandlerRaw (KeyStatusHandlerRaw);
#endif
	s_pThis->m_Screen.Write ("welcome to PegasOS!\n", 21);
	m_Logger.Write (FromKernel, LogNotice, "Just type something!");
	//commenceLogin();
	//PegasosShell->DisplayUserWithDirectory();
	
	CScreenTask *temp;
	// start tasks
	for (unsigned nTaskID = 1; nTaskID <= 4; nTaskID++)
	{
		temp = new CScreenTask (nTaskID, &m_Screen);
		temp->SetWeight(nTaskID);
	}

	new CPrimeTask (&m_Screen);
	new CLEDTask (&m_ActLED);
	
	// this is the main loop for the OS
	for (unsigned nCount = 0; m_ShutdownMode == ShutdownNone; nCount++)
	{
		// CUSBKeyboardDevice::UpdateLEDs() must not be called in interrupt context,
		// that's why this must be done here. This does nothing in raw mode.
		pKeyboard->UpdateLEDs ();

		
		m_Screen.Rotor (0, nCount);
		m_Timer.MsDelay (100);

		
		//main task
		static const char Message[] = "Main ****\n";
		if(CScheduler::Get ()->CScheduler::getPrint()){
			m_Screen.Write (Message, sizeof Message-1);
		}

		m_Event.Clear ();
		m_Timer.StartKernelTimer (.5 * HZ, TimerHandler, this);

		m_Event.Wait ();
		
	}

	// Unmount file system
	if (f_mount (0, DRIVE, 0) != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot unmount drive: %s", DRIVE);
	}

	return m_ShutdownMode;
}

void CKernel::TimerHandler (TKernelTimerHandle hTimer, void *pParam, void *pContext)
{
	CKernel *pThis = (CKernel *) pParam;
	assert (pThis != 0);

	pThis->m_Event.Set ();
}

void CKernel::commenceLogin()
{
	s_pThis->m_Screen.Write("Hello, Welcome to PegasOS!\n Please enter a Username for yourself.\nUsername: ",76);
	s_pThis->m_Timer.MsDelay (5000);
	/*CUSBKeyboardDevice *testKeyboard = (CUSBKeyboardDevice *) s_pThis->m_DeviceNameService.GetDevice ("ukbd1", FALSE);
	if (testKeyboard == 0)
	{
		s_pThis->m_Logger.Write (FromKernel, LogError, "Keyboard not found");
		//return ShutdownHalt;
	}
	#if 1	// set to 0 to test raw mode
		pKeyboard->RegisterShutdownHandler (ShutdownHandler);
		pKeyboard->RegisterKeyPressedHandler (KeyPressedHandler);
	#else
		pKeyboard->RegisterKeyStatusHandlerRaw (KeyStatusHandlerRaw);
	#endif*/
}

void CKernel::KeyPressedHandler (const char *pString)
{
	assert (s_pThis != 0);
	s_pThis->m_Screen.Write (pString, strlen (pString));
	// CommandLineIn(pString);
	PegasosShell->CommandLineIn(pString);
}

void CKernel::ShutdownHandler (void)
{
	assert (s_pThis != 0);
	s_pThis->m_ShutdownMode = ShutdownReboot;
}

void CKernel::KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6])
{
	assert (s_pThis != 0);

	CString Message;
	Message.Format ("Key status (modifiers %02X)", (unsigned) ucModifiers);

	for (unsigned i = 0; i < 6; i++)
	{
		if (RawKeys[i] != 0)
		{
			CString KeyCode;
			KeyCode.Format (" %02X", (unsigned) RawKeys[i]);

			Message.Append (KeyCode);
		}
	}

	s_pThis->m_Logger.Write (FromKernel, LogNotice, Message);
}

//============================================================================//
// PegasOS Kernel Extensions for Shell Commands
void CKernel::SystemReboot()
{
	s_pThis->m_ShutdownMode = ShutdownReboot;
}

void CKernel::SystemOff()
{
	s_pThis->m_ShutdownMode = ShutdownHalt;
}

//============================================================================//
// Getter Extensions for internal kernel devices

CMemorySystem *CKernel::GetKernelMemory()
{
	return &(s_pThis->m_Memory);
}

CActLED *CKernel::GetKernelActLED()
{
	return &(s_pThis->m_ActLED);
}

CDeviceNameService *CKernel::GetKernelDNS()
{
	return &(s_pThis->m_DeviceNameService);
}

CScreenDevice *CKernel::GetKernelScreenDevice()
{
	return &(s_pThis->m_Screen);
}

CSerialDevice *CKernel::GetKernelSerialDevice()
{
	return &(s_pThis->m_Serial);
}

CExceptionHandler *CKernel::GetKernelExceptionHandler()
{
	return &(s_pThis->m_ExceptionHandler);
}

CInterruptSystem *CKernel::GetKernelInterruptSystem()
{
	return &(s_pThis->m_Interrupt);
}

CTimer *CKernel::GetKernelTimer()
{
	return &(s_pThis->m_Timer);
}

CLogger *CKernel::GetKernelLogger()
{
	return &(s_pThis->m_Logger);
}
