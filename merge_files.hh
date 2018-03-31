#ifndef MERGE_FILES_H
#define MERGE_FILES_H

#include "str_record.hh"

class merge_files
{
public:
	merge_files();
	future<> Process(std::vector<sstring> FileNames, size_t _RecordSize);
//	future<> NextFragment(str_record* fragment, sstring destName, size_t destOffset);
private:
#if 0
	struct comp //: public std::binary_function<str_record*, str_record*, bool>
	{
	  bool operator()(str_record* lhs, str_record* rhs) const
	  {
		  std::cout << "merge_files::comp" << std::endl;
		  return lhs->compare( *rhs);
	  }
	};

	std::priority_queue<str_record*, std::vector<str_record*>, comp > pq;
#endif
	std::vector<str_record*> RecordsList;
	size_t RecordSize;
	file DestFile;
	size_t DestOffset;
	//future<> SaveRecord(str_record* val); // other context

	future<> SaveNext();
	future<> SaveRecord(str_record* Rec);
};

#endif // MERGE_FILES_H
