#include "fragment.hh"
#include <boost/range/algorithm/sort.hpp>
#include "core/temporary_buffer.hh"
#include "core/shared_ptr.hh"

fragment::fragment(sstring _DestFileName,
						 size_t _SrcOffset,
						 size_t _FragmentSize,
						 sstring _SrcFileName)
{
	DestOffset = 0;
	DestFileName = _DestFileName;
	FragSrcOffset = _SrcOffset;
	FragmentSize = _FragmentSize;
	SrcFileName = _SrcFileName;
}

future<> fragment::LoadFragment()
{
	int NumOfRecords = FragmentSize/RECORD_SIZE;
	int i;
	str_record* rec;
	size_t SrcOffset = FragSrcOffset;
	for (i=0;i<NumOfRecords;i++) {
		rec = new str_record(SrcFileName);
		rec->SetReadFilePos(SrcOffset);
		SrcOffset+=RECORD_SIZE;
		RecordsList.emplace_back(rec);
	}
	std::cout << "fragment::LoadFragment NumOfRecords=" << NumOfRecords << " vector size=" << RecordsList.size() << std::endl;

	return do_for_each(RecordsList, [this] (str_record* RecItem) {
		return RecItem->LoadRecord(RECORD_SIZE);}).
	then ([this] () {
	if(!(RecordsList.size()>0)) {
	return make_ready_future<>();
}
else
return FlushRecordsList();
});
}

future<> fragment::FlushRecordsList()
{
	std::cout << "sort_fragments::FlushRecordsList --->" << DestFileName << "Src file " << SrcFileName << std::endl;
	return open_file_dma(DestFileName,
								open_flags::rw | open_flags::create | open_flags::truncate).then([this] (file f) mutable {
		std::cout << "sort_fragments::FlushRecordsList" << " open done" << std::endl;
		DestFile = f;
		auto comp = [](str_record* a, str_record* b ) { return a->compare(*b); };
		std::cout << "sort_fragments::FlushRecordsList" << "before sort" << RecordsList.size() << std::endl;
		std::sort(RecordsList.begin(), RecordsList.end(),comp );
		std::cout << "sort_fragments::FlushRecordsList" << " sort done" << std::endl;
#if 0
		/*
				dbgFile = fopen("/tmp/dbg_vector1.bin", "wb");
				fwrite(sortVector.data(), 1, rec->size(), dbgFile);
				fclose(dbgFile);
*/

		std::unique_ptr<char[], free_deleter> tmp;
		tmp = allocate_aligned_buffer<char>(FragmentSize, IO_BUFFER_ALIGN);
		char *Data = tmp.get();
		memcpy(Data, sortVector.data(), FragmentSize);
		/*
				dbgFile = fopen("/tmp/dbg_vector2.bin", "wb");
				fwrite(sortVector.data(), 1, rec->size(), dbgFile);
				fclose(dbgFile);
*/
#endif
		return do_for_each(RecordsList, [this] (str_record* RecItem) {
			// Dma_write looks buggy I tried posix write and I get different files!
			std::cout << "sort_fragments::FlushRecordsList before write offs=" << DestOffset << std::endl;
			if(RecItem->size()>0) {
				//future<> ret = make_ready_future();
				return DestFile.dma_write(DestOffset,/*Data*/RecItem->get_write(), RECORD_SIZE).then([this] (size_t s) {
					//					return f.dma_write(0,sortResult.data(),FragmentSize).then([rec, DestFileName, f] (size_t s) {
					std::cout << "sort_fragments::FlushRecordsList offs=" << DestOffset << "write result=" << s << std::endl;
					DestOffset+=RECORD_SIZE;
					return make_ready_future<>();});
				//return make_ready_future<>();//ret;
			}
			else {
				std::cout << "fragment::FlushRecordsList offs=" << DestOffset << "Empty record" << std::endl;
				return make_ready_future<>();
			}
		}).then([this] () {
		std::cout << "fragment::FlushRecordsList finally"  << std::endl;
		//					return f.dma_write(0,sortResult.data(),FragmentSize).then([rec, DestFileName, f] (size_t s) {
		for(auto&& rec: RecordsList) {
			delete rec;
		}
		RecordsList.clear();

		return make_ready_future<>();
	});
}).finally([this]() {
	std::cout << "str_record::FlushRecordsList finally"<< std::endl;
	return DestFile.close();
});
}

fragment::~fragment()
{
}
