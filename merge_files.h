#ifndef MERGE_FILES_H
#define MERGE_FILES_H

#include <iostream>
#include <queue>
#include "str_record.h"
#include "core/future.hh"

class merge_files
{
public:
	merge_files();
	/*future<>*/void Process(std::vector<sstring> FileNames);
private:
	future<sstring> LoadRecord(sstring fName, uint64_t pos,  size_t size); // other context
	future<> SaveRecord(str_record* val); // other context

};

#endif // MERGE_FILES_H
