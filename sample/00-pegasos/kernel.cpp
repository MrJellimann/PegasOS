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
#include <circle/usb/usbkeyboard.h>
#include <circle/string.h>
#include <circle/util.h>
#include <assert.h>
#include <pegasos/something.h>

#define DRIVE		"SD:"
//#define DRIVE		"USB:"

#define FILENAME	"/circle.txt"

static const char FromKernel[] = "kernel";
int stringLen;
char inputByUser[200], message[]="Command was found!"; /////////
char directory[MAX_DIRECTORY_LENGTH];
char mainCommandName[MAX_DIRECTORY_LENGTH];
char commandParameter[MAX_INPUT_LENGTH];
char userName[MAX_INPUT_LENGTH]="GiancarloGuillen";
char helloMessagePartOne[MAX_INPUT_LENGTH]="Well hello there ";
char helloMessagePartTwo[MAX_INPUT_LENGTH]=", and welcome to PegasOS!";

CKernel *CKernel::s_pThis = 0;
//CShell m_shell;

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
		//m_Logger.Write (FromKernel, LogPanic, "Cannot mount drive: %s", DRIVE);
	}

	if (m_FileSystem.fs_type == FS_EXFAT)
	{
		//m_Logger.Write (FromKernel, LogNotice, "oooooooooooooooooooexfat");
	}

	if (m_FileSystem.fs_type == FS_FAT12)
	{
		//m_Logger.Write (FromKernel, LogNotice, "ooooooooooooooooooooofat12");
	}

	if (m_FileSystem.fs_type == FS_FAT16)
	{
		//m_Logger.Write (FromKernel, LogNotice, "oooooooooooooooooooofat16");
	}

	if (m_FileSystem.fs_type == FS_FAT32)
	{
		//m_Logger.Write (FromKernel, LogNotice, "oooooooooooooooooooooofat32");
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
		//m_Logger.Write (FromKernel, LogPanic, "Cannot create file: %s", FILENAME);
	}

	
	for (unsigned nLine = 1; nLine <= 5; nLine++)
	{
		CString Msg;
		//Msg.Format ("Hello File! (Line %u)\n", nLine);

		unsigned nBytesWritten;
		if (   f_write (&File, (const char *) Msg, Msg.GetLength (), &nBytesWritten) != FR_OK
		    || nBytesWritten != Msg.GetLength ())
		{
			//m_Logger.Write (FromKernel, LogError, "Write error");
			break;
		}
	}

	
	if (f_close (&File) != FR_OK)
	{
		//m_Logger.Write (FromKernel, LogPanic, "Cannot close file");
	}

	// Reopen file, read it and display its contents
	Result = f_open (&File, DRIVE FILENAME, FA_READ | FA_OPEN_EXISTING);
	if (Result != FR_OK)
	{
		//m_Logger.Write (FromKernel, LogPanic, "Cannot open file: %s", FILENAME);
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
	displayUserWithDirectory();
	
	// this is the main loop for the OS
	for (unsigned nCount = 0; m_ShutdownMode == ShutdownNone; nCount++)
	{
		// CUSBKeyboardDevice::UpdateLEDs() must not be called in interrupt context,
		// that's why this must be done here. This does nothing in raw mode.
		pKeyboard->UpdateLEDs ();

		
		m_Screen.Rotor (0, nCount);
		m_Timer.MsDelay (100);
	}

	// Unmount file system
	if (f_mount (0, DRIVE, 0) != FR_OK)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot unmount drive: %s", DRIVE);
	}

	return m_ShutdownMode;
}

void CKernel::KeyPressedHandler (const char *pString)
{
	assert (s_pThis != 0);
	s_pThis->m_Screen.Write (pString, strlen (pString));
	CommandLineIn(pString);
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

void CKernel::displayUserWithDirectory()
{
	char currentLine[MAX_DIRECTORY_LENGTH];
	strcpy(currentLine,getCurrentUserName());
	strcat(currentLine,"@RasberryPI:~$ ");
	//strcat(currentLine,getcwd());
	s_pThis->m_Screen.Write(currentLine,strlen(currentLine));
}

void CKernel::CommandLineIn(const char *keyInput)
{
	assert (s_pThis != 0);
	if(strcmp(keyInput,"\n")==0)
	{
		stringLen=strlen(inputByUser);
		inputByUser[stringLen]='\0';
		splitCommandLine(inputByUser);
		CommandMatch(mainCommandName);
		displayUserWithDirectory();
		strcpy(inputByUser,"");
	}
	if(strcmp(keyInput,"\n")!=0)
		strcat(inputByUser,keyInput);
}

void CKernel::splitCommandLine(const char *inputGiven)
{
	s_pThis->m_Screen.Write("Entered the command with |",26);
	s_pThis->m_Screen.Write(inputGiven,strlen(inputGiven));
	s_pThis->m_Screen.Write("|\n",2);
	strcpy(mainCommandName,"");
	strcpy(commandParameter,"");
	int stringLen=strlen(inputGiven), mainIndex=0, i, j, spacebar=0;
	char charTest[120];
	while(inputGiven[mainIndex]!='\n')
	{
		if(inputGiven[mainIndex]==' ')
		{
			charTest[0]=inputGiven[mainIndex];
			s_pThis->m_Screen.Write("|",1);
			s_pThis->m_Screen.Write(charTest,1);
			s_pThis->m_Screen.Write("|",1);
			spacebar=1;
			s_pThis->m_Screen.Write("Found the fucking space\n",24);
		}
		mainIndex++;
	}
	mainIndex=0;
	if(spacebar==1)
	{
		s_pThis->m_Screen.Write("There is a spacebar!\n",22);
		while(inputGiven[mainIndex]!=' ')
			mainIndex++;
		for(i=0;i<mainIndex;i++)
		{
			mainCommandName[i]=inputGiven[i];
		}
		mainCommandName[mainIndex]='\0';
		s_pThis->m_Screen.Write(mainCommandName,strlen(mainCommandName));
		stringLen=strlen(inputGiven),mainIndex++;
		for(j=0;mainIndex<stringLen;mainIndex++,j++)
		{
			commandParameter[j]=inputGiven[mainIndex];
		}
		commandParameter[mainIndex]='\0';
		s_pThis->m_Screen.Write(commandParameter,strlen(commandParameter));
	}
	if(spacebar==0)
	{
		s_pThis->m_Screen.Write("There is no spacebar!\n",23);
		stringLen=strlen(inputGiven);
		for(mainIndex=0;mainIndex<stringLen;mainIndex++)
			mainCommandName[mainIndex]=inputGiven[mainIndex];
	}
	mainCommandName[mainIndex]='\0';
}

void CKernel::CommandMatch(const char *commandName)
{
	if(strcmp("hello",commandName)==0)
	{
		//strcpy(message,m_shell.Hello("Datboi"));
		strcat(helloMessagePartOne,userName);
		strcat(helloMessagePartOne,helloMessagePartTwo);
		s_pThis->m_Screen.Write(helloMessagePartOne,strlen(helloMessagePartOne));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("changedir",commandName)==0)
	{
		s_pThis->m_Screen.Write(message,strlen(message));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("echo",commandName)==0)
	{
		strcpy(message,"Echo ");
		strcat(message,commandParameter);
		strcat(message," Echo!");
		s_pThis->m_Screen.Write(commandParameter,strlen(commandParameter));
		s_pThis->m_Screen.Write(message,strlen(message));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("copy",commandName)==0)
	{
		s_pThis->m_Screen.Write(message,strlen(message));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("delete",commandName)==0)
	{
		s_pThis->m_Screen.Write(message,strlen(message));
		s_pThis->m_Screen.Write("\n",1);
	}
	//strcpy(mainCommandName,"");
}

char* CKernel::getCurrentUserName()
{
	return userName;
}

void CKernel::CommandParameter(const char *inputString)
{
	int keyIndex=0,length=strlen(inputString);
	//char buffer[20];
	while(inputString[keyIndex]!=' ')
		keyIndex++;
	keyIndex++;
	//s_pThis->m_Screen.Write("Key Index is: ",14);
	for(int i=0; i<length;i++,keyIndex++)
	{
		commandParameter[i]=inputString[keyIndex];
		if(inputString[keyIndex]=='\n')
		{
			commandParameter[i]='\0';
			break;
		}
	}
	//return commandParameter;
}