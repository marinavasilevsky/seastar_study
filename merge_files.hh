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
	struct comp : public std::binary_function<str_record*, str_record*, bool>
	{
	  bool operator()(str_record* lhs, str_record* rhs)
	  {
		  return lhs->compare( *rhs);
	  }
	};

	std::priority_queue<str_record*, std::vector<str_record*>, comp > pq;//(comp);
	std::vector<str_record*> RecordsList;
	future<> SaveRecord(str_record* val); // other context
};

#endif // MERGE_FILES_H
