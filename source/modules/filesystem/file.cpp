#include "common/runtime.h"
#include "wrap_file.h"
#include "filesystem.h"
#include "file.h"

using love::File;

char * File::InitPath(const char * path)
{
	this->fileName = path;
	this->open = false;

	//free(fullPath);

	return nullptr;
}

char * File::Init(const char * name)
{
	char * saveDirectory = love::Filesystem::Instance()->GetSaveDirectory();

	char * fullPath = (char *)malloc(strlen(saveDirectory) + strlen(name) + 1);

	strcpy(fullPath, saveDirectory);
	strcat(fullPath, name);

	return this->InitPath(fullPath);
}

bool File::IsOpen()
{
	return this->open;
}

const char * File::GetMode()
{
	if (this->mode == nullptr)
		return "File is was not opened yet";

	return this->mode;
}

bool File::Open(const char * mode)
{
	this->mode = mode;

	this->fileHandle = fopen(this->fileName, mode);

	if (this->fileHandle)
		this->open = true;

	return this->open;
}

void File::Flush()
{
	fflush(this->fileHandle);
}

void File::Write(const char * data)
{	
	if (!this->open)
		return;

	if (data != NULL)
		fwrite(data, strlen(data) + 1, 1, this->fileHandle);
}

int File::GetSize()
{
	int size = 0;	
	int pos = 0;
	
	if (this->fileHandle == nullptr) 
		return -1;
		
	pos = ftell(this->fileHandle);

	fseek(this->fileHandle, 0, SEEK_END);

	size = ftell(this->fileHandle);

	fseek(this->fileHandle, pos, SEEK_SET);

	return size;
}

const char * File::Read()
{
	char * buffer;
	std::string error;

	if (this->fileHandle == nullptr)
	{
		if (errno == 2)
			error = "File not found";
		else
			error = "File not open for reading";

		return error.c_str();
	}

	buffer = (char *)malloc(this->GetSize() + 1);

	fread(buffer, this->GetSize(), 1, this->fileHandle);

	return buffer;
}

void File::Close()
{
	if (!this->open)
		return;

	fclose(this->fileHandle);
}