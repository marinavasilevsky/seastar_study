#include "merge_files.h"
#include "core/reactor.hh"
#include "core/fstream.hh"
#include <iostream>

merge_files::merge_files()
{

}

future<sstring> merge_files::LoadRecord(sstring fName,uint64_t pos,  size_t size)
{
	//auto buf = static_cast<char*>(::malloc(size));
	//memset(buf, 0, size);
	promise<sstring> retRecord;
	open_file_dma(fName, open_flags::ro).then([pos, size] (file f) mutable {
		//auto rbuf = allocate_aligned_buffer<unsigned char>(4096, 4096);
		//auto rb = rbuf.get();
		const io_priority_class& pc = default_priority_class();

		f.dma_read_bulk<char>(pos, size, pc).then([f] (temporary_buffer<char> buff)  mutable {
			sstring data(buff.get(), buff.size());
			f.close();
			return make_ready_future<sstring>(std::move(data));
		});
	});
	auto fut = retRecord.get_future();
	return std::move(fut);// do_with(std::move(fut), []() {;});
}

/*future<> */ void merge_files::Process(std::vector<sstring> FileNames)
{
	auto comp = []( str_record* a, str_record* b ) { return a->compare(*b); };
	std::priority_queue<str_record*, std::vector<str_record*>, decltype( comp) > pq(comp);

	std::vector<future<>> fs;
}
