#include "merge_files.hh"
#include "core/reactor.hh"
#include "core/fstream.hh"
#include <iostream>

merge_files::merge_files()
{

}

future<> merge_files::Process(std::vector<sstring> FileNames, size_t RecordSize)
{
	std::vector<future<>> fs;

	for (auto&& name: FileNames) {
		str_record* rec = new str_record(name);
		RecordsList.emplace_back(rec);
		fs.emplace_back(std::move(rec->LoadRecord(RecordSize)));
	}

	//auto p = make_shared(std::move(fs));
	return when_all(fs.begin(), fs.end()).then([this](std::vector<future<>> results) {
		//auto comp = []( str_record* a, str_record* b ) { return a->compare(*b); };
		//	std::priority_queue<str_record*, std::vector<str_record*>, decltype( comp) > pq(comp);
		//pq = std::priority_queue<str_record*, std::vector<str_record*>, decltype( comp) >(comp);

		for (auto&& RecordReady: RecordsList) {
			pq.push(RecordReady);
		}

		return make_ready_future<>();
	});
}

#if 0
future<> merge_files::NextFragment(str_record* fragment, sstring destName, size_t destOffset)
{
	return
}
#endif
