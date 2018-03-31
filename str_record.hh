#ifndef STR_RECORD_H
#define STR_RECORD_H

#include "core/reactor.hh"
#include "core/fstream.hh"
#include "core/sstring.hh"
#include <iostream>
#include <queue>
#include "core/future.hh"

using namespace seastar;
#define RECORD_SIZE 4096// Should be configurable

class str_record
{
public:
	str_record(const sstring& fname);
	//str_record();
	//str_record(const str_record &other);
	//const str_record& operator =(const str_record &other);

	uint8_t* get_write() { return (uint8_t*)Str.get_write(); }
	size_t size() { return Str.size(); }

	void SetReadFilePos(uint64_t  val) { ReadFilePos = val; }
	future<> LoadRecord(size_t size); // other context
	bool compare(const str_record &other) const;
	future<bool> IsLoadAvailable(size_t size);
private:
	temporary_buffer<char> Str;
	//sstring Str;
	//std::unique_ptr<char[], free_deleter> Str;
	sstring FileName;
	uint64_t ReadFilePos;
	uint64_t ReadFileSize;
	//void Copy(const str_record &other);
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
