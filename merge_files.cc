#include "merge_files.hh"
#include "core/reactor.hh"
#include "core/fstream.hh"
#include <iostream>

merge_files::merge_files()
{

}

future<> merge_files::Process(std::vector<sstring> FileNames, size_t _RecordSize)
{
	std::cout << "merge_files::Process ---> " << FileNames.size() << std::endl;
	RecordSize = _RecordSize;
	std::vector<future<>> fs;
	str_record* rec;
	for (auto&& name: FileNames) {
		rec = new str_record(name);
		std::cout << "merge_files::Process new rec" << RecordsList.size() << std::endl;
		RecordsList.emplace_back(rec);
		fs.emplace_back(rec->LoadRecord(RecordSize));
	}

	std::cout << "merge_files::Process before load all" << RecordsList.size() << std::endl;
	//auto p = make_shared(std::move(fs));
	return when_all(fs.begin(), fs.end()).then([this](std::vector<future<>> results) {
		//auto comp = []( str_record* a, str_record* b ) { return a->compare(*b); };
		//	std::priority_queue<str_record*, std::vector<str_record*>, decltype( comp) > pq(comp);
		//pq = std::priority_queue<str_record*, std::vector<str_record*>, comp >(comp);
		std::cout << "merge_files::Process after load all size=" << RecordsList.size() << "results" << results.size() << std::endl;
#if 0
		for (auto&& RecordReady: RecordsList) {
			std::cout << "merge_files::Process push" << std::endl;
			if(RecordReady == NULL) {
				std::cout << "merge_files::Process NULL rec" << std::endl;
			}
			pq.push(RecordReady);
		}
#endif
		auto comp = [](str_record* a, str_record* b ) { return (!(a->compare(*b))); };
		std::sort(RecordsList.begin(), RecordsList.end(),comp );
		std::cout << "merge_files::Process" << " sort done" << std::endl;
		return SaveNext();
	});
}

future<> merge_files::SaveNext()
{
	std::cout << "merge_files::SaveNext --->" << std::endl;
	if(RecordsList.empty())
		return make_ready_future<>();
	else {
		str_record* RecSave = RecordsList.back();
		RecordsList.pop_back();
		return SaveRecord(RecSave).then ([this, RecSave] () {
			return RecSave->IsLoadAvailable(RecordSize).then([this, RecSave] (bool ready) {
				if(!ready) {
					delete RecSave;
					return SaveNext();
				}
				else {
					return RecSave->LoadRecord(RecordSize).then([this, RecSave] () {
						RecordsList.emplace_back(RecSave);
						return SaveNext();
					});
				}
			});
		});
	}
}

future<> merge_files::SaveRecord(str_record* Rec)
{
	std::cout << "merge_files::SaveRecord --->" << std::endl;
	return open_file_dma("/tmp/out.bin",
								open_flags::rw | open_flags::create).then([this, Rec] (file f) mutable {
		std::cout << "sort_fragments::FlushRecordsList" << " open done" << std::endl;
		DestFile = f;

		return DestFile.dma_write(DestOffset,/*Data*/Rec->get_write(), RecordSize).then([this] (size_t s) {
			//					return f.dma_write(0,sortResult.data(),FragmentSize).then([rec, DestFileName, f] (size_t s) {
			std::cout << "merge_files::SaveRecord offs=" << DestOffset << "write result=" << s << std::endl;
			DestOffset+=RecordSize;
			return DestFile.close().then ([] () {
				return make_ready_future<>();
			});
		});
	});
}
