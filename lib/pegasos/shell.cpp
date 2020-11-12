#include <circle/usb/usbkeyboard.h>
#include <circle/string.h>
#include <circle/util.h>
#include <assert.h>
#include <pegasos/something.h>
#include <pegasos/shell.h>

CKernel *pKernel = 0;
PShell *PShell::s_pThis = 0;

static const char _FromKernel[] = "kernel";
int _stringLen, _globalIndex=0;
char _inputByUser[200], _message[PMAX_INPUT_LENGTH]="Command was found!"; /////////
char _directory[PMAX_DIRECTORY_LENGTH];
char _mainCommandName[PMAX_DIRECTORY_LENGTH];
char _commandParameter[PMAX_INPUT_LENGTH];
char _userName[PMAX_INPUT_LENGTH] = "GiancarloGuillen";
char _helloMessagePartOne[PMAX_INPUT_LENGTH] = "Well hello there ";
char _helloMessagePartTwo[PMAX_INPUT_LENGTH] = ", and welcome to PegasOS!";
char _helpMessage1[] = "This is a list of the Commands for PegasOS:\n\tbackgroundpalette\n\tcd\n\tclear\n\tconcat\n\tcopy\n\tcreatedir";
char _helpMessage2[] = "\n\tcreatefile\n\tcurrentdir\n\tdelete\n\tdeletedir\n\techo\n\tfilespace\n\tfind\n\thead\n\thello\n\thelp\n\tlogin\n\tmount\n\tmove\n\tpower\n\tsysteminfo\n\ttail";
char _helpMessage3[] = "\n\tmount\n\tmove\n\tpower\n\tsysteminfo\n\ttail\n\ttasklist\n\ttermiantetask\n\ttextpalette\n\tuninstall\n";
FIL _NewFIle;

PShell::PShell(void)
{
    s_pThis = this;
}

PShell::~PShell(void)
{
    s_pThis = 0;
}

void PShell::AssignKernel(CKernel* _kernel)
{
    assert (_kernel != 0);

    pKernel = _kernel;
}

void PShell::CommandLineIn(const char* keyInput)
{
    // pKernel->GetKernelLogger()->Write(_FromKernel, LogNotice, "\nAttempting to read char into PSHELL...\n");
	// assert (pKernel != 0);
    // pKernel->GetKernelLogger()->Write(_FromKernel, LogNotice, "Successfully asserted kernel exists.\n");

	if (strcmp(keyInput, "\n") == 0)
	{
		_stringLen = strlen(_inputByUser);
		_inputByUser[_stringLen] = '\0';
		SplitCommandLine(_inputByUser);
		CommandMatch(_mainCommandName);
		DisplayUserWithDirectory();
		strcpy(_inputByUser, "");
	}
	if (strcmp(keyInput,"\n") != 0)
		strcat(_inputByUser, keyInput);
}

void PShell::SplitCommandLine(const char* input)
{
	strcpy(_mainCommandName, "");
	strcpy(_commandParameter, "");
	int  mainIndex = 0, subIndex = 0, spacebar = 0, charValue;

	for (int x = 0; x < strlen(input); x++)
	{
		if (input[x] == 32)
			spacebar = 1;
	}

	if (spacebar == 1)
	{
		for (int x = 0; x < strlen(input); x++)
		{
			if (input[x] == 32)
				spacebar = x;
		}
		for (int x = 0; x < spacebar; x++, mainIndex++)
		{
			_mainCommandName[mainIndex] = input[mainIndex];
		}

		_mainCommandName[mainIndex] = '\0';
		mainIndex++;

		for (; subIndex<strlen(input); subIndex++, mainIndex++)
		{
			_commandParameter[subIndex] = input[mainIndex];
		}
		_commandParameter[subIndex] = '\0';
	}
	else if (spacebar == 0)
	{
		strcpy(_mainCommandName, input);
	}
}

void PShell::CommandMatch(const char *commandName)
{
    // Change Directory
	if (strcmp("changedir", commandName) == 0)
	{
        assert(pKernel != 0);
		pKernel->GetKernelScreenDevice()->Write(_message, strlen(_message));
		pKernel->GetKernelScreenDevice()->Write("\n", 1);
	}
    // Create File
	else if (strcmp("createfile", commandName) == 0)
	{
		char fileName[] = "";
		strcat(fileName, "SD:/");
		strcat(fileName, _commandParameter);
		FRESULT Result = f_open (&_NewFIle, fileName, FA_WRITE | FA_CREATE_ALWAYS);
		if (Result != FR_OK)
		{
			//pKernel->m_Screen.Write("Can not create file");
			pKernel->GetKernelLogger()->Write(_FromKernel, LogPanic, "Cannot create file %s", _NewFIle);
			//m_Logger.Write (_FromKernel, LogPanic, "Cannot create file: %s", FILENAME);
		}
	
		for (unsigned nLine = 1; nLine <= 5; nLine++)
		{
			CString Msg;
			Msg.Format ("Hello File! (Line %u)\n", nLine);

			unsigned nBytesWritten;
			if (f_write(&_NewFIle, (const char *) Msg, Msg.GetLength (), &nBytesWritten) != FR_OK|| nBytesWritten != Msg.GetLength ())
			{
				pKernel->GetKernelLogger()->Write (_FromKernel, LogError, "Write error");
				break;
			}
		}

		
		if (f_close (&_NewFIle) != FR_OK)
		{
			pKernel->GetKernelLogger()->Write (_FromKernel, LogPanic, "Cannot close file");
		}
	}
    // Copy
	else if (strcmp("copy", commandName) == 0)
	{
		pKernel->GetKernelScreenDevice()->Write(_message, strlen(_message));
		pKernel->GetKernelScreenDevice()->Write("\n", 1);
	}
    // Current Directory
	else if (strcmp("currentdir", commandName) == 0)
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

				pKernel->GetKernelScreenDevice()->Write ((const char *) FileName, FileName.GetLength ());

				if (i % 4 == 3)
				{
					pKernel->GetKernelScreenDevice()->Write ("\n", 1);
				}
			}

			Result = f_findnext (&Directory, &FileInfo);
		}
	}
    // Delete
	else if (strcmp("delete", commandName) == 0)
	{
		pKernel->GetKernelScreenDevice()->Write(_message, strlen(_message));
		pKernel->GetKernelScreenDevice()->Write("\n", 1);
	}
    // Echo
	else if (strcmp("echo", commandName) == 0)
	{
		strcpy(_message, "Echo ");
		strcat(_message, _commandParameter);
		strcat(_message, " Echo!");
		
		pKernel->GetKernelScreenDevice()->Write(_message, strlen(_message));
		pKernel->GetKernelScreenDevice()->Write("\n", 1);
	}
    // Hello
	else if (strcmp("hello", commandName) == 0)
	{
		//strcpy(message,m_shell.Hello("Datboi"));
		strcat(_helloMessagePartOne, _userName);
		strcat(_helloMessagePartOne, _helloMessagePartTwo);
		pKernel->GetKernelScreenDevice()->Write(_helloMessagePartOne, strlen(_helloMessagePartOne));
		pKernel->GetKernelScreenDevice()->Write("\n", 1);
	}
    // Help
	else if (strcmp("help", commandName) == 0)
	{
		pKernel->GetKernelScreenDevice()->Write(_helpMessage1, strlen(_helpMessage1));
		pKernel->GetKernelScreenDevice()->Write(_helpMessage2, strlen(_helpMessage2));
		pKernel->GetKernelScreenDevice()->Write(_helpMessage3, strlen(_helpMessage3));
	}
    // Login
	else if (strcmp("login", commandName) == 0)
	{
		pKernel->GetKernelScreenDevice()->Write("We found login!\n", 16);
	}

	strcpy(_mainCommandName, "");
}

void PShell::DisplayUserWithDirectory()
{
	char currentLine[PMAX_DIRECTORY_LENGTH];
	strcpy(currentLine, GetCurrentUsername());
	strcat(currentLine, "@RasberryPI:~$ ");
	//strcat(currentLine,getcwd());
	pKernel->GetKernelScreenDevice()->Write(currentLine, strlen(currentLine));
}

char* PShell::GetCurrentUsername()
{
	return _userName;
}