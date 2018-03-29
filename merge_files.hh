#ifndef MERGE_FILES_H
#define MERGE_FILES_H

#include "str_record.hh"

class merge_files
{
public:
	merge_files();
	future<> Process(std::vector<sstring> FileNames, size_t RecordSize);
//	future<> NextFragment(str_record* fragment, sstring destName, size_t destOffset);
private:
	future<> SaveRecord(str_record* val); // other context

};

#endif // MERGE_FILES_H
