#include <circle/usb/usbkeyboard.h>
#include <circle/string.h>
#include <circle/util.h>
#include <assert.h>
#include <pegasos/something.h>
#include <pegasos/shell.h>

#define SHELLDRIVE		"SD:/"

CKernel *pKernel = 0;
PShell *PShell::s_pThis = 0;

static const char _FromKernel[] = "kernel";

int _stringLen, _globalIndex=0, dirRed=31, dirGreen=31, dirBlue=31, userRed=31, userGreen=31, userBlue=31;//, _OffBoot=0;
char _inputByUser[200], _message[PMAX_INPUT_LENGTH]="Command was found!"; /////////
char _directory[PMAX_DIRECTORY_LENGTH]="SD:";
char _mainCommandName[PMAX_DIRECTORY_LENGTH];
char _commandParameterOne[PMAX_INPUT_LENGTH];
char _commandParameterTwo[PMAX_INPUT_LENGTH];
char _userName[PMAX_INPUT_LENGTH] = "GiancarloGuillen";
char _helloMessagePartOne[PMAX_INPUT_LENGTH] = "Well hello there ";
char _helloMessagePartTwo[PMAX_INPUT_LENGTH] = ", and welcome to PegasOS!";
char _helpMessage1[PMAX_INPUT_LENGTH] = "This is a list of the Commands for PegasOS:\n\tbackgroundpalette\n\tchangedir\n\tclear\n\tconcat\n\tcopy\n\tcreatedir";
char _helpMessage2[PMAX_INPUT_LENGTH] = "\n\tcreatefile\n\tdelete\n\tdeletedir\n\tdisplaytasks\n\techo\n\tfilespace\n\tfind\n\thead\n\thello\n\thelp\n\tlistdir\n\tlogin\n\tmount\n\tmove";
char _helpMessage3[PMAX_INPUT_LENGTH] = "\n\tpower\n\tsysteminfo\n\ttail\n\ttasklist\n\ttermiantetask\n\ttextpalette\n";
FIL _NewFIle, _ReadFile;
TScreenColor color;
TScreenStatus stat;

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
	strcpy(_commandParameterOne, "");
	strcpy(_commandParameterTwo, "");
	int  mainIndex = 0, subIndex = 0, spacebar = 0, stringLength=strlen(input);
	for (int x = 0; x < stringLength; x++)
	{
		if (input[x] == 32)
			spacebar += 1;
	}

	if (spacebar == 1)
	{
		for (int x = 0; x < stringLength; x++)
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

		for (; subIndex<stringLength; subIndex++, mainIndex++)
		{
			_commandParameterOne[subIndex] = input[mainIndex];
		}
		_commandParameterOne[subIndex] = '\0';
	}
	else if (spacebar == 0)
	{
		strcpy(_mainCommandName, input);
	}
	else if(spacebar==2)
	{
		spacebar=0;
		for (int x = 0; x < stringLength; x++)
		{
			if (input[x] == 32)
			{
				spacebar = x;
				break;
			}
		}
		for (int x = 0; x < spacebar; x++, mainIndex++)
		{
			_mainCommandName[mainIndex] = input[mainIndex];
		}
		_mainCommandName[mainIndex] = '\0';
		mainIndex++;
		for (int x = mainIndex; x < stringLength; x++)
		{
			if (input[x] == 32)
			{
				spacebar = x;
				break;
			}
		}
		for (int x = 0; mainIndex < spacebar; x++, mainIndex++)
		{
			_commandParameterOne[x] = input[mainIndex];
		}
		_commandParameterOne[mainIndex] = '\0';
		mainIndex++;
		for (; subIndex<stringLength; subIndex++, mainIndex++)
		{
			_commandParameterTwo[subIndex] = input[mainIndex];
		}
		_commandParameterTwo[subIndex] = '\0';
	}
}

void PShell::CommandMatch(const char *commandName)
{
    // Change Directory
	if (strcmp("changedir", commandName) == 0)
	{
        assert(pKernel != 0);
    
    // 		pKernel->GetKernelScreenDevice()->Write(_message, strlen(_message));
    // 		pKernel->GetKernelScreenDevice()->Write("\n", 1);
    
		char _FilePath[MAX_DIRECTORY_LENGTH] = "";
		
		if (strcmp(_commandParameterOne, "") != 0)
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
			else if(_Result == FR_OK) // was just if before
			{
				strcpy(_directory,_FilePath);
				FixWorkingDirectory();
				//pKernel->GetKernelScreenDevice()->Write(, strlen(currentLine));
			}
		}
		
	}
    // Create File
	else if (strcmp("createfile", commandName) == 0)
	{
		char fileName[] = "";
		strcat(fileName, _directory);
		strcat(fileName,"/");
		strcat(fileName, _commandParameterOne);
    
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
	// Create Directory
	else if (strcmp("createdir", commandName) == 0)
	{
		assert(pKernel != 0);
		char _FilePath[]="";
		strcat(_FilePath,_directory);
		strcat(_FilePath,"/");
		strcat(_FilePath,_commandParameterOne);
		//DIR Directory;
		FRESULT _Result=f_mkdir(_FilePath);
		if (_Result != FR_OK)
		{
			pKernel->GetKernelScreenDevice()->Write("The sub-directory wasn't able to be made.\n", 42);
		}
	}
	// Clear
	else if (strcmp("clear", commandName) == 0)
	{
		assert(pKernel != 0);
	}
    // Concat
	else if (strcmp("concat", commandName) == 0)
	{
		assert(pKernel != 0);
	}
	// Copy
	else if (strcmp("copy", commandName) == 0)
	{
		assert(pKernel != 0);
		int check;
		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"We entered head command with |%s| |%s|\n",_commandParameterOne,_commandParameterTwo);
		char mainFileName[] = "", newFileName[MAX_INPUT_LENGTH]="", buffer[MAX_INPUT_LENGTH]="";
		strcpy(mainFileName,_directory);
		strcat(mainFileName,"/");
		strcat(mainFileName,_commandParameterOne);
		strcpy(newFileName,_commandParameterTwo);
		strcat(newFileName,"/");
		strcat(newFileName,_commandParameterOne);
		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"The mainFileName is: |%s| The newFileName is: |%s|\n",mainFileName,newFileName);
		FRESULT mainResult = f_open (&_ReadFile, mainFileName, FA_READ | FA_OPEN_EXISTING);
		FRESULT newResult = f_open (&_NewFIle, newFileName, FA_WRITE | FA_CREATE_ALWAYS);
		if (mainResult != FR_OK)
		{
			pKernel->GetKernelLogger()->Write(_FromKernel, LogWarning, "Cannot open file: %s", mainFileName);
		}
		if (newResult != FR_OK)
		{
			pKernel->GetKernelLogger()->Write(_FromKernel, LogWarning, "Cannot open file: %s", newFileName);
		}
		while(f_gets(buffer,100,&_ReadFile)!=nullptr)
		{
			//pKernel->GetKernelScreenDevice()->Write(buffer,strlen(buffer));
			check=f_puts(buffer,&_NewFIle);
		}
		if (f_close (&_NewFIle) != FR_OK)
		{
			pKernel->GetKernelLogger()->Write (_FromKernel, LogWarning, "Cannot close file");
		}
	}
    // Current Directory
	else if (strcmp("listdir", commandName) == 0)
	{
		pKernel->GetKernelScreenDevice()->Write("Current Working Directory is: ",31);
		pKernel->GetKernelScreenDevice()->Write(_directory,strlen(_directory));
		pKernel->GetKernelScreenDevice()->Write("\n",1);
		char buffer[MAX_INPUT_LENGTH]="";
		DIR Directory;
		FILINFO FileInfo;
		FRESULT Result = f_findfirst (&Directory, &FileInfo, _directory, "*");
    
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
	else if ((strcmp("delete", commandName) == 0) || (strcmp("deletedir", commandName) == 0))
	{
		assert(pKernel != 0);
		char _FilePath[]="";
		strcat(_FilePath,_directory);
		strcat(_FilePath,"/");
		strcat(_FilePath,_commandParameterOne);
		//DIR Directory;
		FRESULT _Result=f_unlink(_FilePath);
		if ((_Result != FR_OK) && (strcmp("delete", commandName) == 0))
		{
			pKernel->GetKernelScreenDevice()->Write("The file was not deleted.\n", 26);
		}
		if ((_Result != FR_OK) && (strcmp("deletedir", commandName) == 0))
		{
			pKernel->GetKernelScreenDevice()->Write("The directory was not deleted.\n", 31);
		}
	}
	// Change the Directory color
	else if(strcmp("dirtext",commandName)==0)
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
				dirRed=atoi(digits);
			if(x==1)
				dirGreen=atoi(digits);
			if(x==2)
				dirBlue=atoi(digits);
			sub=0,index++;
			strcpy(digits,"");
		}
		SetColor(dirRed,dirGreen,dirBlue);
	}
    // Echo
	else if (strcmp("echo", commandName) == 0)
	{
		strcpy(_message, "Echo ");
		strcat(_message, _commandParameterOne);
		strcat(_message, " Echo!");
		
		pKernel->GetKernelScreenDevice()->Write(_message, strlen(_message));
		pKernel->GetKernelScreenDevice()->Write("\n", 1);
	}
	// Head
	else if(strcmp("head", commandName)==0)
	{
		assert(pKernel != 0);
		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"We entered head command with |%s| |%s|\n",_commandParameterOne,_commandParameterTwo);
		int _LinesToBeRead=5, _LinesRead=0;
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
			pKernel->GetKernelScreenDevice()->Write(buffer,strlen(buffer));
			if (_LinesRead==_LinesToBeRead)
			{
				break;
			}
			_LinesRead+=1;
		}
		if (f_close (&_NewFIle) != FR_OK)
		{
			pKernel->GetKernelLogger()->Write (_FromKernel, LogWarning, "Cannot close file");
		}
	}
    // Hello
	else if (strcmp("hello", commandName) == 0)
	{
		//strcpy(message,m_shell.Hello("Datboi"));
		strcat(_helloMessagePartOne, _userName);
		strcat(_helloMessagePartOne, _helloMessagePartTwo);
		pKernel->GetKernelScreenDevice()->Write(_helloMessagePartOne, strlen(_helloMessagePartOne));
		pKernel->GetKernelScreenDevice()->Write("\n", 1);
		strcpy(_helloMessagePartOne,"Well hello there ");
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
	// Move
	else if (strcmp("move", commandName) == 0)
	{
		assert(pKernel != 0);
		int check;
		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"We entered head command with |%s| |%s|\n",_commandParameterOne,_commandParameterTwo);
		// mainFileName -> file you're moving
		// newFileName -> destination you're moving to
		char mainFileName[MAX_DIRECTORY_LENGTH] = "", newFileName[MAX_INPUT_LENGTH] = "", buffer[MAX_INPUT_LENGTH] = "";
		
		strcpy(mainFileName,_directory);
		strcat(mainFileName,"/");
		strcat(mainFileName,_commandParameterOne);

		strcpy(newFileName,_commandParameterTwo);
		strcat(newFileName,"/");
		strcat(newFileName,_commandParameterOne);

		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"The mainFileName is: |%s| The newFileName is: |%s|\n",mainFileName,newFileName);
		FRESULT mainResult = f_open (&_ReadFile, mainFileName, FA_READ | FA_OPEN_EXISTING);
		if (mainResult != FR_OK)
		{
			pKernel->GetKernelLogger()->Write(_FromKernel, LogWarning, "Cannot open file: %s", mainFileName);
		}
		else
		{
			FRESULT newResult = f_open (&_NewFIle, newFileName, FA_WRITE | FA_CREATE_ALWAYS);
		
			if (newResult != FR_OK)
			{
				pKernel->GetKernelLogger()->Write(_FromKernel, LogWarning, "Cannot open file: %s", newFileName);
			}
		}
		
		while (f_gets(buffer,100,&_ReadFile) != nullptr)
		{
			//pKernel->GetKernelScreenDevice()->Write(buffer,strlen(buffer));
			check = f_puts(buffer, &_NewFIle);
		}
		if (f_close (&_NewFIle) != FR_OK)
		{
			pKernel->GetKernelLogger()->Write (_FromKernel, LogWarning, "Cannot close file");
		}
		FRESULT deleteResult=f_unlink(mainFileName);
		if ((deleteResult != FR_OK) && (strcmp("delete", commandName) == 0))
		{
			pKernel->GetKernelScreenDevice()->Write("The file was not deleted.\n", 26);
		}
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
  	// Display Tasks/Scheduler Demo
	else if (strcmp("displaytasks", commandName) == 0)
  	{
		CScheduler::Get ()->CScheduler::turnPrintOn();
	}
	// Stop printing Scheduler Demo
	else if (strcmp("s", commandName) == 0)
  	{
		CScheduler::Get ()->CScheduler::turnPrintOff();
	}
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

	// strcpy(_mainCommandName, "");
	// strcpy(_commandParameterOne, "");
	// strcpy(_commandParameterTwo, "");

	memset(_mainCommandName, 0, sizeof(_mainCommandName));
	memset(_commandParameterOne, 0, sizeof(_commandParameterOne));
	memset(_commandParameterTwo, 0, sizeof(_commandParameterTwo));
}

void PShell::DisplayUserWithDirectory()
{
	char currentLine[PMAX_DIRECTORY_LENGTH];
	strcpy(currentLine,_userName);
	SetColor(userRed,userGreen,userBlue);
	pKernel->GetKernelScreenDevice()->Write(currentLine, strlen(currentLine));
	strcpy(currentLine, "@RasberryPI:");
	strcat(currentLine,_directory);
	strcat(currentLine,"$ ");
	SetColor(dirRed,dirGreen,dirBlue);
	//strcat(currentLine,getcwd());
	pKernel->GetKernelScreenDevice()->Write(currentLine, strlen(currentLine));
	SetColor(31,31,31);
}

void PShell::EditUserName(const char *loginName)
{
	strcpy(_directory,"SD:/users/");
	strcat(_directory,loginName);
	strcpy(_userName,loginName);
}

void PShell::FixWorkingDirectory()
{
	int index=0, currentAmount=0, amountSlash=0, length=strlen(_directory), amountDot=0;
	char temp[MAX_INPUT_LENGTH]="";
	//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"The directory variable is: |%s|",_directory);
	//	This finds the number ".."
	while (index<length)
	{
		if(_directory[index]=='.')
		{
			if(_directory[index+1]=='.')
			{
				amountDot+=1;
			}
		}
		index++;
	}
	//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"The amountDot is : %d",amountDot);
	index=0;
	//	This finds the first instance of ".."
	while (index<length)
	{
		if(_directory[index]=='/')
		{
			amountSlash+=1;
		}
		index++;
	}
	//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"The amountSlash is : %d",amountSlash);
	strcpy(temp,_directory);
	//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"Insanity check: |%s|\n",temp);
	while(amountDot>0)
	{
		index=0;
		//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"Entered the first fixing loop loop\n");
		while(currentAmount!=(amountSlash-amountDot))
		{
			//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"Entered the second fixing loop\n");
			if(temp[index]=='/')
			{
				currentAmount+=1;
				//break;
			}
			index+=1;
		}
		temp[index-1]='\0';
		amountDot--;
	}
	//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"Index is %d\n",index);
	//pKernel->GetKernelLogger()->Write(_FromKernel,LogDebug,"The final result for temp is: |%s|\n",temp);
	strcpy(_directory,temp);
}

void PShell::SetColor(int red, int green, int blue)
{
	color = COLOR16(red,green,blue); 	
	stat = pKernel->GetKernelScreenDevice()->GetStatus();
	stat.Color = color;
	if(pKernel->GetKernelScreenDevice()->SetStatus(stat)==false)
	{
		pKernel->GetKernelLogger()->Write(_FromKernel, LogDebug, "Screen stat just returned false");
	}
}