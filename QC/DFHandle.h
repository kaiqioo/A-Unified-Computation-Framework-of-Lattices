/*
 * The DFHandle class is used to manipulate the tuples in a data file.
 *
 * DF is short for DataFile :)
 *
 */

#ifndef DFHANDLE_H
#define DFHANDLE_H 1

class DFHandle
{
private:
	// Is the file handle open?
	bool isOpen;
	// Name of the data file.
	char *fileName;
	// The opened FILE pointer.
	FILE *fPointer;
	// Current tuple id, start from 1.
	TID currTid;
	
public:
	DFHandle ();
	~DFHandle ();

	// Attaches a file to this handle
	bool Open (const char* file);
	// Detaches file from this handle
	bool Close ();
	// Reposition the file stream to the beginning of the file.
	bool ReWind ();
	// Fetch next tuple in data file.
	bool GetNextTuple (TupleHandle &tHandle);
};

#endif
