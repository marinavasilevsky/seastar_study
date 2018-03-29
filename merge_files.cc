#include "merge_files.h"
#include "core/reactor.hh"
#include "core/fstream.hh"
#include <iostream>

merge_files::merge_files()
{

}

/*future<> */ void merge_files::Process(std::vector<sstring> FileNames)
{
	auto comp = []( str_record* a, str_record* b ) { return a->compare(*b); };
	std::priority_queue<str_record*, std::vector<str_record*>, decltype( comp) > pq(comp);

	std::vector<future<>> fs;

}
