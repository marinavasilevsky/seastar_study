#ifndef STR_RECORD_H
#define STR_RECORD_H

#include "core/sstring.hh"
#include <iostream>
#include <queue>
#include "core/future.hh"

using namespace seastar;

class str_record
{
public:
	str_record(const sstring& fname);
	const sstring& GetStr() const { return Str; }
	future<> LoadRecord(size_t size); // other context
	bool compare(const str_record &other) const;
private:
	sstring Str;
	sstring FileName;
	uint64_t ReadFilePos;
};

struct CompareRecord : public std::binary_function<str_record*, str_record*, bool>
{
	bool operator()(const str_record& lhs, const str_record& rhs) const
	{
		bool b = lhs.compare(rhs);
		return b;
	}
};


#endif // STR_RECORD_H
