#ifndef STR_RECORD_H
#define STR_RECORD_H

#include "core/sstring.hh"

using namespace seastar;

class str_record
{
public:
	str_record(const sstring& s,const sstring& fname);
	const sstring& GetStr() const { return Str; }
	bool compare(const str_record &other) const;
private:
	sstring Str;
	sstring FileName;
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
