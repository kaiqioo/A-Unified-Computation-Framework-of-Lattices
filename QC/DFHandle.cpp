#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "TupleHandle.h"
#include "DFHandle.h"

DFHandle::DFHandle ()
{
	isOpen = false;
	fileName = NULL;
	fPointer = NULL;
	currTid = 1;
}

DFHandle::~DFHandle ()
{
	if (isOpen) 
		Close ();
}

bool DFHandle::Open (const char *file)
{
	if (isOpen)
		Close();

	fileName = strdup (file);
	fPointer = fopen (fileName, "r");
	if (fPointer == NULL)
	{
		perror ("fopen error!");
		return false;
	}
	// Now the file has been successfully opened.
	isOpen = true;

	return true;
}

bool DFHandle::Close ()
{
	if (!isOpen) return false;

	if (fclose (fPointer) == EOF)
	{
		perror ("fclose error!");
		return false;
	}

	free (fileName);
	fileName = NULL;
	fPointer = NULL;
	currTid = 1;
	isOpen = false;
	
	return true;
}

bool DFHandle::ReWind ()
{
	if (!isOpen) return false;

	rewind (fPointer);
	return true;
}

// pre:  tHandle is a reference to a TupleHandle object, 
//       which must have been created by the client.
// post: A newly fetch tuple stored in tHandle.
//       Returns false if some error occurs.
bool DFHandle::GetNextTuple (TupleHandle &tHandle)
{
	if (!isOpen) return false;

	static char tData[MAXTUPLELEN];
	// Get next tuple in data file.
	if (fgets (tData, MAXTUPLELEN, fPointer) != NULL)
	{
		// strip '\n'
		tData[strlen (tData) - 1] = '\0';
		tHandle.Open (tData, sizeof (tData), currTid);
		currTid++;
		return true;
	}
	return false;
}
