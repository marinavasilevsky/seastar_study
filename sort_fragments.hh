#ifndef FRAGMENT_SRC_FILE_H
#define FRAGMENT_SRC_FILE_H

#include "core/sstring.hh"
#include <iostream>
#include <queue>
#include "core/future.hh"
#include "core/file.hh"
#include "str_record.hh"

using namespace seastar;
class sort_fragments
{
public:
	sort_fragments();
	future<std::vector<sstring>> CreateFragments(size_t _FragmentSize, sstring _SrcFileName, sstring _outDirName);

private:
	std::vector<sstring> FragmentsList;
	size_t FragmentSize;
	size_t SrcOffset;
	sstring SrcFileName;
	sstring OutDirName;
};

#endif // FRAGMENT_SRC_FILE_H
