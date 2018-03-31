#ifndef FRAGMENT_HH
#define FRAGMENT_HH

#include "core/sstring.hh"
#include <iostream>
#include <queue>
#include "core/future.hh"
#include "core/file.hh"
#include "str_record.hh"

class fragment
{
public:
	fragment(sstring _DestFileName, size_t _SrcOffset,
				size_t _FragmentSize,
				sstring _SrcFileName);

	future<> LoadFragment();
	future<> FlushRecordsList();
	~fragment();
private:
	std::vector<str_record*> RecordsList;
	file DestFile;
	size_t DestOffset;
	sstring DestFileName;
	size_t FragSrcOffset;
	size_t FragmentSize;
	sstring SrcFileName;
};

#endif // FRAGMENT_HH
