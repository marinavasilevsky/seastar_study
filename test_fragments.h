#ifndef TEST_FRAGMENTS_H
#define TEST_FRAGMENTS_H

#include "core/sstring.hh"
#include <iostream>
#include <queue>
#include "core/future.hh"

using namespace seastar;
class test_fragments
{
public:
	test_fragments();
	future<std::vector<sstring>> CreateFiles(int NumOfFiles, size_t size);
	future<> CreateFragment(sstring name, size_t size);
};

#endif // TEST_FRAGMENTS_H
