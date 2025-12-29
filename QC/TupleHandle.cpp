#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "TupleHandle.h"


TupleHandle::TupleHandle (const char *delim)
{
	assert (delim);
	currFid = 0;
	isValid = false;
	isFirstCall = true;
	Delimiter = strdup(delim);
}

TupleHandle::~TupleHandle () 
{
	free (Delimiter);
}

void TupleHandle::Open (const char * const pData, size_t length, TID tid)
{
	assert (length <= MAXTUPLELEN);
	strcpy (tData, pData);
	Tid = tid;
	isValid = true;
	isFirstCall = true;
	currFid = 0;
}

bool TupleHandle::GetTid (TID &tid) const
{
	if (!isValid) return false;
	tid = this->Tid;
	return true;
}

bool TupleHandle::GetData (const char *&pData) const
{
	if (!isValid) return false;

	pData = &tData[0];
	return true;
}

bool TupleHandle::GetNextField (char * const pField, 
								 size_t length, FID &fid)
{
	assert (pField != NULL);
	assert (length > 0);

	if (!isValid) return false;

	char *ptr = NULL;
	if (isFirstCall)
	{
		ptr = strtok (tData, Delimiter);
		isFirstCall = false;
	}
	else
		ptr = strtok (NULL, Delimiter);
		
	if (ptr)
	{
		size_t len = strlen (ptr);
		// Keep a character for '\0'
		length--;
		// If length <= len, only copy the first length characters.
		len = length > len ? len : length;
		strncpy (pField, ptr, len);
		pField[len] = '\0';
		fid = currFid++;
		return true;
	}
	return false;
}

