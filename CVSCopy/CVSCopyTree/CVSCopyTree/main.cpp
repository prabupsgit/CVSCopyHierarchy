#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <string>
#include <Windows.h>

using namespace std;

void printUsage(void)
{
	printf("Usage:");
	printf("CVSCopyTree <source> <destination>\n");
}

int CreateTarget(string dir)
{
	char* tok;
	char* dest = strdup(dir.c_str() + 3);
	int ret = 0;
	string drive = dir.substr(0, 2);

	while((tok = strtok(dest, "\\/")) != NULL)
	{
		dest = NULL;
		drive.append("/");
		drive.append(tok);
		BOOL err = CreateDirectory(drive.c_str(), NULL);
		if(err == ERROR_ALREADY_EXISTS || err == ERROR_SUCCESS || err == 1)
			continue;
		else
		{
			ret = -1;
			break;
		}
	}

	free(dest);
	return ret;
}


DWORD CopyDirectory(const char* source, const char* destination)
{
	WIN32_FIND_DATA fHandle;
	HANDLE hFind = NULL;
	int ret = 0;

	string lSource(source);
	
	if((hFind = FindFirstFile(lSource.c_str(), &fHandle)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", lSource.c_str());
		return GetLastError();
    }

	if(fHandle.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		string lSource(string(source) + "/*.*");
	
		if((hFind = FindFirstFile(lSource.c_str(), &fHandle)) == INVALID_HANDLE_VALUE)
		{
			printf("Path not found: [%s]\n", lSource.c_str());
			return GetLastError();
		}
		
		do
		{
			char sPath[2048]; memset(sPath, 0, sizeof(sPath));
			ret = 0;
			if(strcmp(fHandle.cFileName, ".") != 0
					&& strcmp(fHandle.cFileName, "..") != 0)
			{
				sprintf(sPath, "%s/%s", source, fHandle.cFileName);
				ret = CopyDirectory(sPath, destination);
				if(ret != 0)
					break;
			}
		}
		while(FindNextFile(hFind, &fHandle));
	}
	else
	{
		string dest(destination);
		dest.append("/"); 
		
		string folderHierarchy(source + 3);	
		
		size_t fNameLen = strlen(fHandle.cFileName);
		folderHierarchy.erase(folderHierarchy.end() - fNameLen, folderHierarchy.end());

		dest.append(folderHierarchy); dest.append("/");

		int ret = CreateTarget(dest);

		if(ret == 0)
		{
			dest.append(string(fHandle.cFileName));		
			ret = CopyFile(lSource.c_str(), dest.c_str(), false)?0:1;
		}
		else
			printf("Unable to create target directory [%s]", dest.c_str());
	}

	FindClose(hFind);
	return ret;
}

int main(int argc, char** argv)
{

	if(argc < 3)
	{
		printUsage();
		getch();
		return -1;
	}

	if(strlen(argv[1]) < 3 || argv[1][1] != ':')
	{
		printf("Source path [%s] invalid. It must be of format <drive>:\\<path>\n", argv[1]);
		getch();
		return -1;
	}

	if(CopyDirectory(argv[1], argv[2]) != 0)
	{
		printf("Error during CopyDirectory()..\n");
		getch();
	}

	return 0;
}