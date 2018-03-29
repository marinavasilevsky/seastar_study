#ifndef MERGE_FILES_H
#define MERGE_FILES_H

#include "str_record.h"

class merge_files
{
public:
	merge_files();
	/*future<>*/void Process(std::vector<sstring> FileNames);
private:
	future<> SaveRecord(str_record* val); // other context

};

#endif // MERGE_FILES_H
