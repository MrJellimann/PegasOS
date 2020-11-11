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
int stringLen, globalIndex=0;
char inputByUser[200], message[MAX_INPUT_LENGTH]="Command was found!"; /////////
char directory[MAX_DIRECTORY_LENGTH];
char mainCommandName[MAX_DIRECTORY_LENGTH];
char commandParameter[MAX_INPUT_LENGTH];
char userName[MAX_INPUT_LENGTH]="GiancarloGuillen";
char helloMessagePartOne[MAX_INPUT_LENGTH]="Well hello there ";
char helloMessagePartTwo[MAX_INPUT_LENGTH]=", and welcome to PegasOS!";
char helpMessage1[]="This is a list of the Commands for PegasOS:\n\tbackgroundpalette\n\tcd\n\tclear\n\tconcat\n\tcopy\n\tcreatedir";
char helpMessage2[]="\n\tcreatefile\n\tcurrentdir\n\tdelete\n\tdeletedir\n\techo\n\tfilespace\n\tfind\n\thead\n\thello\n\thelp\n\tlogin\n\tmount\n\tmove\n\tpower\n\tsysteminfo\n\ttail";
char helpMessage3[]="\n\tmount\n\tmove\n\tpower\n\tsysteminfo\n\ttail\n\ttasklist\n\ttermiantetask\n\ttextpalette\n\tuninstall\n";
FIL NewFIle;

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
	CommandLineIn(pString);
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

void CKernel::splitCommandLine(const char *inputGiven)
{
	//s_pThis->m_Screen.Write("Entered the command with |",26);
	//s_pThis->m_Screen.Write(inputGiven,strlen(inputGiven));
	//s_pThis->m_Screen.Write("|\n",2);
	strcpy(mainCommandName,"");
	strcpy(commandParameter,"");
	int  mainIndex=0, subIndex=0, spacebar=0, charValue;
	for(int x=0; x<strlen(inputGiven); x++)
	{
		if(inputGiven[x]==32)
			spacebar=1;
	}
	if(spacebar==1)
	{
		for(int x=0; x<strlen(inputGiven); x++)
		{
			if(inputGiven[x]==32)
				spacebar=x;
		}
		for(int x=0; x<spacebar; x++, mainIndex++)
		{
			mainCommandName[mainIndex]=inputGiven[mainIndex];
		}
		mainCommandName[mainIndex]='\0';
		//s_pThis->m_Screen.Write("Lets see if this works now: |",29);
		//s_pThis->m_Screen.Write(mainCommandName,strlen(mainCommandName));
		//s_pThis->m_Screen.Write("|\n",2);
		mainIndex++;
		for(; subIndex<strlen(inputGiven); subIndex++, mainIndex++)
		{
			commandParameter[subIndex]=inputGiven[mainIndex];
		}
		commandParameter[subIndex]='\0';
		//s_pThis->m_Screen.Write("Lets see if this works now: |",29);
		//s_pThis->m_Screen.Write(commandParameter,strlen(commandParameter));
		//s_pThis->m_Screen.Write("|\n",2);
	}
	else if(spacebar==0)
	{
		strcpy(mainCommandName,inputGiven);
		//s_pThis->m_Screen.Write("Lets see if this works now: |",29);
		//s_pThis->m_Screen.Write(mainCommandName,strlen(mainCommandName));
		//s_pThis->m_Screen.Write("|\n",2);
	}
}

void CKernel::CommandMatch(const char *commandName)
{
	if(strcmp("changedir",commandName)==0)
	{
		s_pThis->m_Screen.Write(message,strlen(message));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("createfile",commandName)==0)
	{
		char fileName[]="";
		strcat(fileName,"SD:/");
		strcat(fileName,commandParameter);
		FRESULT Result = f_open (&NewFIle, fileName, FA_WRITE | FA_CREATE_ALWAYS);
		if (Result != FR_OK)
		{
			//s_pThis->m_Screen.Write("Can not create file");
			s_pThis->m_Logger.Write(FromKernel, LogPanic, "Cannot create file %s",NewFIle);
			//m_Logger.Write (FromKernel, LogPanic, "Cannot create file: %s", FILENAME);
		}
	
		for (unsigned nLine = 1; nLine <= 5; nLine++)
		{
			CString Msg;
			Msg.Format ("Hello File! (Line %u)\n", nLine);

			unsigned nBytesWritten;
			if (f_write(&NewFIle, (const char *) Msg, Msg.GetLength (), &nBytesWritten) != FR_OK|| nBytesWritten != Msg.GetLength ())
			{
				s_pThis->m_Logger.Write (FromKernel, LogError, "Write error");
				break;
			}
		}

		
		if (f_close (&NewFIle) != FR_OK)
		{
			s_pThis->m_Logger.Write (FromKernel, LogPanic, "Cannot close file");
		}
		}
	else if(strcmp("copy",commandName)==0)
	{
		s_pThis->m_Screen.Write(message,strlen(message));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("currentdir",commandName)==0)
	{
		DIR Directory;
		FILINFO FileInfo;
		FRESULT Result = f_findfirst (&Directory, &FileInfo, DRIVE "/", "*");
		for (unsigned i = 0; Result == FR_OK && FileInfo.fname[0]; i++)
		{
			if (!(FileInfo.fattrib & (AM_HID | AM_SYS)))
			{
				CString FileName;
				FileName.Format ("%-19s", FileInfo.fname);

				s_pThis->m_Screen.Write ((const char *) FileName, FileName.GetLength ());

				if (i % 4 == 3)
				{
					s_pThis->m_Screen.Write ("\n", 1);
				}
			}

			Result = f_findnext (&Directory, &FileInfo);
		}
	}
	else if(strcmp("delete",commandName)==0)
	{
		s_pThis->m_Screen.Write(message,strlen(message));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("echo",commandName)==0)
	{
		strcpy(message,"Echo ");
		strcat(message,commandParameter);
		strcat(message," Echo!");
		//s_pThis->m_Screen.Write(commandParameter,strlen(commandParameter));
		s_pThis->m_Screen.Write(message,strlen(message));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("hello",commandName)==0)
	{
		//strcpy(message,m_shell.Hello("Datboi"));
		strcat(helloMessagePartOne,userName);
		strcat(helloMessagePartOne,helloMessagePartTwo);
		s_pThis->m_Screen.Write(helloMessagePartOne,strlen(helloMessagePartOne));
		s_pThis->m_Screen.Write("\n",1);
	}
	else if(strcmp("help",commandName)==0)
	{
		s_pThis->m_Screen.Write(helpMessage1,strlen(helpMessage1));
		s_pThis->m_Screen.Write(helpMessage2,strlen(helpMessage2));
		s_pThis->m_Screen.Write(helpMessage3,strlen(helpMessage3));
	}
	else if(strcmp("login",commandName)==0)
	{
		s_pThis->m_Screen.Write("We found login!\n",16);
	}
	strcpy(mainCommandName,"");
}

char* CKernel::getCurrentUserName()
{
	return userName;
}

/*
	List of all the functions in PegasOS:
	backgroundpalette
	cd
	clear
	tconcat
	copy
	createdir
	createfile
	currentdir
	delete
	deletedir
	echo
	filespace
	find
	head
	hello
	help
	login
	mount
	move
	power
	tail
	mount
	move
	power
	systeminfo
	tail
	tasklist
	termiantetask
	textpalette
	uninstall
*/
