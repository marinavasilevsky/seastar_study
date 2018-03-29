#ifndef FRAGMENT_SRC_FILE_H
#define FRAGMENT_SRC_FILE_H

#include "core/sstring.hh"
#include <iostream>
#include <queue>
#include "core/future.hh"

using namespace seastar;
class sort_fragments
{
public:
	sort_fragments();
	future<std::vector<sstring>> CreateFragments(size_t FragmentSize, sstring SrcFileName, sstring outDirName);

private:
	future<> LoadFragment(size_t FragmentSize, sstring SrcFileName, size_t SrcOffset, sstring DestFileName);
};

#endif // FRAGMENT_SRC_FILE_H
