/*
 * The TupleHandle class defines tuple objects. To materialize a tuple, a
 * client creates an instance of this class and passes it to DFHandle methods
 * that reads a tuple.
 */

#ifndef TUPLEHANDLE_H
#define TUPLEHANDLE_H 1

// Define tuple id type.
typedef unsigned int TID;
// Define field id type.
typedef unsigned int FID;
// Maximum length of a tuple.
const unsigned int MAXTUPLELEN = 4096;

class TupleHandle
{
private:
	// Is this tuple Handle valid?
	bool isValid;
	// The tuple ID.
	TID Tid;
	// Current Field ID. Start from 0.
	FID currFid;
	// The tuple data.
	char tData[MAXTUPLELEN];
	// Field delimiter.
	char *Delimiter;
	// First call to GetNextField?
	bool isFirstCall;
public:
	TupleHandle (const char *delim);
	~TupleHandle ();

	void Open (const char * const pData, size_t length, TID tid);
	// Return this tuple ID.
	bool GetTid (TID &tid) const;
	// Set pData points to tData.
	bool GetData (const char *&pData) const;
	// Make a copy of next Field to pFiled.
	bool GetNextField (char * const pField, size_t length, FID &fid);
};


#endif
