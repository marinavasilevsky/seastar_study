#include "str_record.h"

str_record::str_record(const sstring &s, const sstring &fname) : Str(s), FileName(fname)
{}

bool str_record::compare(const str_record &other) const
{
	int ret = Str.compare(other.GetStr());
	if(ret <= 0)
		return true;
	return false;
}
