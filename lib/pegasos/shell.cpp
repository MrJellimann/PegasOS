#include <circle/usb/usbkeyboard.h>
#include <circle/string.h>
#include <circle/util.h>
#include <assert.h>
#include <pegasos/something.h>
#include <pegasos/shell.h>

CKernel *pKernel = 0;
PShell *PShell::s_pThis = 0;

static const char _FromKernel[] = "kernel";
<<<<<<< HEAD
int _stringLen, _globalIndex=0, dirRed=31, dirGreen=31, dirBlue=31, userRed=31, userGreen=31, userBlue=31;//, _OffBoot=0;
=======
int _stringLen, _globalIndex=0;
>>>>>>> parent of 0fe681b... Merge branch 'master' into revzet_branch
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
<<<<<<< HEAD
		char _FilePath[]="";
		if(strcmp(_commandParameterOne,"")!=0)
		{
			strcat(_FilePath,_directory);
			strcat(_FilePath,"/");
			strcat(_FilePath,_commandParameterOne);
			FRESULT _Result=f_chdir(_FilePath);
			if (_Result != FR_OK)
			{
				pKernel->GetKernelScreenDevice()->Write("The file path was incorrect\n", 28);
				//pKernel->GetKernelScreenDevice()->Write(_FilePath,strlen(_FilePath));
			}
			if(_Result == FR_OK)
			{
				strcpy(_directory,_FilePath);
				FixWorkingDirectory();
				//pKernel->GetKernelScreenDevice()->Write(, strlen(currentLine));
			}
		}
		
		
=======
		pKernel->GetKernelScreenDevice()->Write(_message, strlen(_message));
		pKernel->GetKernelScreenDevice()->Write("\n", 1);
>>>>>>> parent of 0fe681b... Merge branch 'master' into revzet_branch
	}
    // Create File
	else if (strcmp("createfile", commandName) == 0)
	{
		char fileName[] = "";
<<<<<<< HEAD
		strcat(fileName, _directory);
		strcat(fileName,"/");
		strcat(fileName, _commandParameterOne);
=======
		strcat(fileName, "SD:/");
		strcat(fileName, _commandParameter);
>>>>>>> parent of 0fe681b... Merge branch 'master' into revzet_branch
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
<<<<<<< HEAD
		FRESULT Result = f_findfirst (&Directory, &FileInfo, _directory, "*");
=======
		FRESULT Result = f_findfirst (&Directory, &FileInfo, DRIVE "/", "*");
>>>>>>> parent of 0fe681b... Merge branch 'master' into revzet_branch
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
    // Reboot
    else if (strcmp("reboot", commandName) == 0)
    {
        pKernel->SystemReboot();
    }
    // Power Off
    else if (strcmp("power", commandName) == 0)
    {
        pKernel->SystemOff();
    }
<<<<<<< HEAD
	// Tail
	else if (strcmp("tail", commandName) == 0)
	{
		assert(pKernel != 0);
		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"We entered head command with |%s| |%s|\n",_commandParameterOne,_commandParameterTwo);
		int _LinesToBeRead=5, _LinesRead=0, _LinesTotal=0;
		if(strcmp(_commandParameterTwo,"")!=0)
		{
			_LinesToBeRead=atoi(_commandParameterTwo);
		}
		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"_LinesToBeRead is: |%d|\n",_LinesToBeRead);
		char fileName[] = "", buffer[MAX_INPUT_LENGTH]="";
		strcpy(fileName,_directory);
		strcat(fileName,"/");
		strcat(fileName,_commandParameterOne);
		//pKernel->GetKernelScreenDevice()->Write(fileName,strlen(fileName));
		//pKernel->GetKernelScreenDevice()->Write("\n",1);
		FRESULT Result = f_open (&_NewFIle, fileName, FA_READ | FA_OPEN_EXISTING);
		if (Result != FR_OK)
		{
			pKernel->GetKernelLogger()->Write(_FromKernel, LogWarning, "Cannot open file: %s", fileName);
		}
		while(f_gets(buffer,100,&_NewFIle)!=nullptr)
		{
			_LinesTotal+=1;
		}
		if (f_close (&_NewFIle) != FR_OK)
		{
			pKernel->GetKernelLogger()->Write (_FromKernel, LogWarning, "Cannot close file");
		}
		Result = f_open (&_NewFIle, fileName, FA_READ | FA_OPEN_EXISTING);
		if (Result != FR_OK)
		{
			pKernel->GetKernelLogger()->Write(_FromKernel, LogWarning, "Cannot open file: %s", fileName);
		}
		while(f_gets(buffer,100,&_NewFIle)!=nullptr)
		{
			if(_LinesRead>=(_LinesTotal-_LinesToBeRead))
			{
				pKernel->GetKernelScreenDevice()->Write(buffer,strlen(buffer));
			}
			_LinesRead+=1;
		}
		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"_LineTotal is: |%d|\n",_LinesTotal);
		if (f_close (&_NewFIle) != FR_OK)
		{
			pKernel->GetKernelLogger()->Write (_FromKernel, LogWarning, "Cannot close file");
		}
	}
	// Change the Username color
	else if(strcmp("usertext",commandName)==0)
	{
		assert(pKernel != 0);
		char temp[PMAX_INPUT_LENGTH]="", digits[PMAX_INPUT_LENGTH]="";
		strcpy(temp,_commandParameterOne);
		int index = 0, sub = 0;
		for(int x=0;x<3;x++)
		{
			//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"Entered the for loop with %d",x);
			while(index<strlen(temp))
			{
				if(temp[index]==',')
					break;
				//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"Entered the while loop with temp[%d]=%c",index,temp[index]);
				digits[sub]=temp[index];
				sub++,index++;
			}
			digits[sub]='\0';
			if(x==0)
				userRed=atoi(digits);
			if(x==1)
				userGreen=atoi(digits);
			if(x==2)
				userBlue=atoi(digits);
			sub=0,index++;
			strcpy(digits,"");
		}
		SetColor(userRed,userGreen,userBlue);
	}

=======
	else if(strcmp("displaytasks", commandName) == 0){
		CScheduler::Get ()->CScheduler::turnPrintOn();
	}
	else if(strcmp("s", commandName) == 0){
		CScheduler::Get ()->CScheduler::turnPrintOff();
	}


>>>>>>> parent of 0fe681b... Merge branch 'master' into revzet_branch
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