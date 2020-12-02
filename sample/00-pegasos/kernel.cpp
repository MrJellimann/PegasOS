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
int _OffBoot=0, _UserNameMatch=0, _PasswordMatch=0;
char _inputUsername[MAX_INPUT_LENGTH]="", _inputPassword[MAX_INPUT_LENGTH]="", _userResponse[MAX_INPUT_LENGTH]="";

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

	// Mount Keyboard
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
	
	// Start Task System
	CScreenTask *temp;
	// start tasks
	for (unsigned nTaskID = 1; nTaskID <= 4; nTaskID++)
	{
		temp = new CScreenTask (nTaskID, &m_Screen);
		// m_Logger.Write(FromKernel, LogNotice, "ScreenTask %i Addr: %x", nTaskID, temp);
		temp->SetWeight(nTaskID);
	}

	new CPrimeTask (&m_Screen);
	new CLEDTask (&m_ActLED);
  
  	// Start login
	s_pThis->m_Screen.Write("Hello, Welcome to PegasOS!\nPlease login in to continue...\nUsername:  ", 69);
	
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
		//m_Logger.Write (FromKernel, LogPanic, "Cannot unmount drive: %s", DRIVE);
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
	char userDirectory[] = "SD:/users", _tempFileName[] = "";
	char Buffer[100];

	// First, check for the 'users' directory
	FRESULT _userDir = f_mkdir(userDirectory);

	if (_userDir != FR_EXIST)
	{
		if (_userDir != FR_OK)
		{
			s_pThis->m_Logger.Write (FromKernel, LogPanic, "Cannot create 'users' directory. Please restart the system.");
		}
	}

	if (_OffBoot == 0)
	{
		DIR Directory;
		FILINFO FileInfo;
		FRESULT Result = f_findfirst (&Directory, &FileInfo, userDirectory, "*");
		for (unsigned i = 0; Result == FR_OK && FileInfo.fname[0]; i++)
		{
			if (!(FileInfo.fattrib & (AM_HID | AM_SYS)))
			{
				CString FileName;
				FileName.Format ("%-19s", FileInfo.fname);
				strcpy(_tempFileName,FileName);
				EditFileName(_tempFileName);
				
				if(strcmp(_inputUsername,_tempFileName)==0)
				{
					_UserNameMatch=1;
				}
			}

			Result = f_findnext (&Directory, &FileInfo);
		}
		if(_UserNameMatch==1)
		{
			s_pThis->m_Screen.Write ("Password:  ",12);
			_OffBoot=2;
		}
		else if(_UserNameMatch==0)
		{
			s_pThis->m_Screen.Write ("User does not exist. Would you like to create one?  [Y]es/[N]o   ", 66);
			strcpy(_inputUsername, "");
			_OffBoot = 1;
		}
	}
	else if( _OffBoot == 1)
	{
		if ((strcmp(_userResponse, "no") == 0) || (strcmp(_userResponse, "n") == 0))
		{
			s_pThis->m_Screen.Write("Re-enter Username:  ",21);
			_OffBoot = 0;
		}
		else if ((strcmp(_userResponse,"yes") == 0) || (strcmp(_userResponse,"y") == 0))
		{
			s_pThis->m_Screen.Write("Enter desired Username:  ",26);
			_OffBoot = 3;
		}
		strcpy(_userResponse,"");
	}
	else if (_OffBoot == 2)
	{
		strcat(userDirectory, "/");
		strcat(userDirectory, _inputUsername);
		strcat(userDirectory, "/");
		strcat(userDirectory, _inputUsername);
		strcat(userDirectory, ".txt");

		FIL passwordFILE;
		FRESULT Result = f_open (&passwordFILE, userDirectory, FA_READ | FA_OPEN_EXISTING);
		
		if (Result != FR_OK)
		{
			s_pThis->m_Screen.Write("Cannot open the file!\n",22);
		}
		
		unsigned nBytesRead;
		while ((Result = f_read (&passwordFILE, Buffer, sizeof Buffer, &nBytesRead)) == FR_OK)
		{
			if (nBytesRead < sizeof Buffer)		// EOF?
			{
				break;
			}
		}

		if (strcmp(Buffer, _inputPassword) == 0)
		{
			s_pThis->m_Screen.Write("\nSuccessfully logged in!\n",25);

			// First, try to move into the user's 'desktop'
			strcpy(userDirectory, "SD:/users/");
			strcat(userDirectory, _inputUsername);
			strcat(userDirectory, "/desktop");

			// FRESULT _desktop = f_mkdir(userDirectory);
			f_mkdir(userDirectory);
			FRESULT _Result = f_chdir(userDirectory);
			if (_Result != FR_OK)
			{
				s_pThis->m_Screen.Write("The file path was incorrect\n", 28);

				// If that fails, move into the user's root directory
				strcpy(userDirectory, "SD:/users/");
				strcat(userDirectory, _inputUsername);

				FRESULT _Result2 = f_chdir(userDirectory);
				if (_Result2 != FR_OK)
				{
					s_pThis->m_Screen.Write("The file path was incorrect\n", 28);
				}
			}

			PegasosShell->EditUserName(_inputUsername);
			PegasosShell->DisplayUserWithDirectory();
			_OffBoot = 5;
		}
		else if (strcmp(Buffer, _inputPassword) != 0)
		{
			s_pThis->m_Screen.Write("Password mismatch.\n",21);
			s_pThis->m_Screen.Write("Re-enter password:  ",21);
			strcpy(_inputPassword,"");
			_OffBoot=2;
		}
		if (f_close (&passwordFILE) != FR_OK)
		{
			s_pThis->m_Screen.Write("Cannot close the file!\n",22);
		}
	}
	else if (_OffBoot == 3)
	{
		// This will create the directory and user file.
		// Create /users/<user>
		strcpy(userDirectory, "SD:/users/");
		strcat(userDirectory, _inputUsername);
		FRESULT _Result = f_mkdir(userDirectory);
		// Create /users/<user>/documents
		strcat(userDirectory, "/documents");
		FRESULT _Result2 = f_mkdir(userDirectory);
		// Create /users/<user>/desktop
		strcpy(userDirectory, "SD:/users/");
		strcat(userDirectory, _inputUsername);
		strcat(userDirectory, "/desktop");
		FRESULT _Result3 = f_mkdir(userDirectory);

		if (_Result != FR_OK)
		{
			s_pThis->m_Screen.Write("The sub-directory wasn't able to be made.\n", 42);
		}
		
		if (_Result2 != FR_OK)
		{
			s_pThis->m_Screen.Write("The user's 'documents' directory wasn't able to be made.\n", 57);
		}

		if (_Result3 != FR_OK)
		{
			s_pThis->m_Screen.Write("The user's 'desktop' directory wasn't able to be made.\n", 57);
		}

		s_pThis->m_Screen.Write("Username was created. Now please enter a password: ",52);
		_OffBoot = 4;
	}
	else if (_OffBoot == 4)	//This will take the password given and then write it to their file and then log them in
	{
		strcat(userDirectory,"/");
		strcat(userDirectory,_inputUsername);
		strcat(userDirectory,"/");
		strcat(userDirectory,_inputUsername);
		strcat(userDirectory,".txt");
		FIL _NewFIle;
		int Pass = 1;

		FRESULT Result = f_open (&_NewFIle, userDirectory, FA_WRITE | FA_CREATE_ALWAYS);
		if (Result != FR_OK)
		{
			s_pThis->m_Screen.Write("Cannot create user file.\n",25);
			Pass = 0;
		}

		CString Msg;
		Msg.Format ("%s", _inputPassword);
		unsigned nBytesWritten;
		if (f_write(&_NewFIle, (const char *) Msg, Msg.GetLength (), &nBytesWritten) != FR_OK|| nBytesWritten != Msg.GetLength ())
		{
			s_pThis->m_Screen.Write("Cannot write to user file.\n",25);
			Pass = 0;
		}
		
		if (f_close (&_NewFIle) != FR_OK)
		{
			s_pThis->m_Screen.Write("Cannot close user file.\n",24);
			Pass = 0;
		}

		if (Pass == 1)
		{
			s_pThis->m_Screen.Write("\nSuccessfully logged in!\n", 25);

			// First, try to move into the user's 'desktop'
			strcpy(userDirectory, "SD:/users/");
			strcat(userDirectory, _inputUsername);
			strcat(userDirectory, "/desktop");

			FRESULT _Result = f_chdir(userDirectory);
			if (_Result != FR_OK)
			{
				s_pThis->m_Screen.Write("The file path was incorrect\n", 28);

				// If that fails, move into the user's root directory
				strcpy(userDirectory, "SD:/users/");
				strcat(userDirectory, _inputUsername);

				FRESULT _Result2 = f_chdir(userDirectory);
				if (_Result2 != FR_OK)
				{
					s_pThis->m_Screen.Write("The file path was incorrect\n", 28);
				}
			}

			PegasosShell->EditUserName(_inputUsername);
			PegasosShell->DisplayUserWithDirectory();
			_OffBoot = 5;
		}
	}

}

void CKernel::EditFileName(char* tempFileName)
{
	int length=strlen(tempFileName), index=0;
	while(index<length)
	{
		if(tempFileName[index]==32)
		{
			tempFileName[index]='\0';
			break;
		}
		index++;
	}
}

void CKernel::KeyPressedHandler (const char *pString)
{
	assert (s_pThis != 0);
  
	if (_OffBoot != 2 && _OffBoot != 4)
		s_pThis->m_Screen.Write (pString, strlen (pString));
	if (_OffBoot != 5)
	{
		LoginInput(pString);
	}
	else if (_OffBoot == 5)
	{
		PegasosShell->CommandLineIn(pString);
	}

	// s_pThis->m_Logger.Write (FromKernel, LogNotice, "%i, %c", pString[0], pString[0]);
}

void CKernel::LoginInput(const char* keyInput)
{
	int stringLength = 0;

	int _inputLen = strlen(keyInput);
	int _len2;

	for (int i = 0; i < _inputLen; i++)
	{
		switch ((char)keyInput[i])
		{
			case '\n': // Carriage Return
				switch (_OffBoot)
				{
					case 0: // Input Username
					case 3:
						stringLength = strlen(_inputUsername);
						_inputUsername[stringLength] = '\0';
						commenceLogin();
						break;
					
					case 1: // User Response
						stringLength = strlen(_userResponse);
						_userResponse[stringLength] = '\0';
						commenceLogin();
						break;
					
					case 2: // Input Password
					case 4:
						stringLength = strlen(_inputPassword);
						_inputPassword[stringLength] = '\0';
						commenceLogin();
						break;

					default:
						// Do nothing
						break;
				}
				break;
			
			case '\b': // Backspace
			case 127: // Delete
				switch (_OffBoot)
				{
					case 0: // Input Username
					case 3:
						_len2 = strlen(_inputUsername);
						_inputUsername[_len2-1] = '\0';
						break;
					
					case 1: // User Response
						_len2 = strlen(_userResponse);
						_userResponse[_len2-1] = '\0';
						break;
					
					case 2: // Input Password
					case 4:
						_len2 = strlen(_inputPassword);
						_inputPassword[_len2-1] = '\0';
						break;
					
					default:
						// Do nothing
						break;
				}
				break;
			
			case '\x1b': // Escape Character
				// Skip the rest of the escape string
				i = _inputLen;
				break;
			
			default: // Other Characters
				switch (_OffBoot)
				{
					case 0: // Input Username
					case 3:
						strcat(_inputUsername, &keyInput[i]);
						break;

					case 1: // User Response
						strcat(_userResponse, &keyInput[i]);
						break;

					case 2: // Input Password
					case 4:
						strcat(_inputPassword, &keyInput[i]);
						break;
					
					default:
						// Do nothing
						break;
				}
				break;
		}
	}
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

CScheduler *CKernel::GetKernelScheduler()
{
	return &(s_pThis->m_Scheduler);
}

CSynchronizationEvent *CKernel::GetKernelSyncEvent()
{
	return &(s_pThis->m_Event);
}

CMachineInfo *CKernel::GetKernelInfo()
{
	return &(s_pThis->m_Info);
}