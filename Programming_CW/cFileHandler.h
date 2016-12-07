/*
=================
cFileHandler.h
- Header file for class definition - SPECIFICATION
- Header file for the Sprite class
=================
*/
#ifndef _CFILEHANDLER_H
#define _CFILEHANDLER_H

#include "GameConstants.h"
#include <fstream>

class cFileHandler
{
private:
	string m_filename;
	fstream theFile;

public:
	cFileHandler();
	cFileHandler(string fileName);

	int openFile(int mode); 
	void closeFile();
	string readDataFromFile();
	void writeDataToFile(string textLine);
	void setFileName(string aFilename);
	string getFileName();
};

#endif
