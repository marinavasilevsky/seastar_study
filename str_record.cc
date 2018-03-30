#include "str_record.hh"

str_record::str_record(const sstring &fname) :FileName(fname),  ReadFilePos(0)
{}

bool str_record::compare(const str_record &other) const
{
	return Str.operator ==(other.Str);
}

future<> str_record::LoadRecord(size_t size)
{
	return open_file_dma(FileName, open_flags::ro).then([size, this] (file f) mutable {
		return f.dma_read_bulk<char>(ReadFilePos,size).then([f, this, size] (temporary_buffer<char> buff)  mutable {
			if(!buff.empty())
				Str = std::move(buff);
			else
				Str = temporary_buffer<char>();
			f.close();
			ReadFilePos+=size;
			return make_ready_future<>();
		});
	});
	//auto fut = retRecord.get_future();
	//return std::move(fut);// do_with(std::move(fut), []() {;});
}

