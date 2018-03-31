#include "sort_fragments.hh"
#include "test_fragments.hh"
#include "str_record.hh"
#include "core/sstring.hh"
#include <boost/range/algorithm/sort.hpp>
#include "core/temporary_buffer.hh"
#include "core/shared_ptr.hh"

sort_fragments::sort_fragments()
{

}


future<std::vector<sstring>> sort_fragments::CreateFragments(size_t _FragmentSize, sstring _SrcFileName, sstring _outDirName)
{
	FragmentSize = _FragmentSize;
	SrcFileName = _SrcFileName;
	OutDirName = _outDirName;

	std::cout << "sort_fragments::CreateFragments "<< OutDirName << std::endl;

	return open_file_dma(SrcFileName, open_flags::ro).then ( [this] (file f) mutable {
		std::cout << "sort_fragments::CreateFragments File open" << std::endl;

		return do_with(std::move(f), [this](file& f) {
			 return f.size().then([&f, this](uint64_t FileSize) {
				 return f.close().then ([this, FileSize] () {
					 std::cout << "sort_fragments::CreateFragments File size" << FileSize << std::endl;
					 int NumOfFragments = FileSize/FragmentSize;
					 int i;

					 std::cout << "sort_fragments::CreateFragments NumOfFragments" << NumOfFragments << std::endl;
					 sstring name;

					 for (i=0;i<NumOfFragments;i++) {
						 std::cout << "sort_fragments::CreateFragments i " << i << "  " << OutDirName << std::endl;
						 name = OutDirName+"/frag"+ to_sstring(i)+".bin";
						 FragmentsList.emplace_back(name);
						 std::cout << "sort_fragments::CreateFragments add " << name  << std::endl;
					 }

					 name = FragmentsList.front();
					 std::cout << "sort_fragments::CreateFragments Before Load " << name  << std::endl;
					 return LoadFragment( 0, name).then([this] () {
						 return make_ready_future<std::vector<sstring>>(FragmentsList);
					 });
				 });
			 });
		});
	});
}

future<> sort_fragments::LoadFragment(size_t SrcOffset, sstring DestFileName)
{
	std::vector<str_record*> RecordsList;

	int NumOfRecords = FragmentSize/RECORD_SIZE;
	int i;
	str_record* rec;
	for (i=0;i<NumOfRecords;i++) {
		rec = new str_record(SrcFileName);
		rec->SetReadFilePos(SrcOffset);
		SrcOffset+=RECORD_SIZE;
		RecordsList.emplace_back(rec);
	}
	std::cout << "sort_fragments::LoadFragment NumOfRecords=" << NumOfRecords << " vector size=" << RecordsList.size() << std::endl;

	return do_for_each(RecordsList, [this] (str_record* RecItem) {
		return RecItem->LoadRecord(RECORD_SIZE); }).then ([RecordsList, DestFileName,this] () mutable {

		std::cout << "sort_fragments::LoadFragment --->" << DestFileName << "Src file " << SrcFileName << std::endl;
		return open_file_dma(DestFileName,
								open_flags::rw | open_flags::create | open_flags::truncate).then([RecordsList, DestFileName] (file f) mutable {
			std::cout << "sort_fragments::LoadFragment" << DestFileName << " open done" << std::endl;

			if(RecordsList.size()>0) {
				auto comp = [](str_record* a, str_record* b ) { return a->compare(*b); };
				std::cout << "sort_fragments::LoadFragment" << "before sort" << RecordsList.size() << std::endl;
				std::sort(RecordsList.begin(), RecordsList.end(),comp );
				std::cout << "sort_fragments::LoadFragment" << " sort done" << std::endl;
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
				size_t DestOffset = 0;
				return do_for_each(RecordsList, [&DestOffset, &f] (str_record* RecItem) {
					// Dma_write looks buggy I tried posix write and I get different files!
					if(RecItem->size()>0) {
						//future<> ret = make_ready_future();
						return f.dma_write(DestOffset,/*Data*/RecItem->get_write(), RECORD_SIZE).then([&DestOffset] (size_t s) {
							//					return f.dma_write(0,sortResult.data(),FragmentSize).then([rec, DestFileName, f] (size_t s) {
							std::cout << "sort_fragments::LoadFragment offs=" << DestOffset << "write result=" << s << std::endl;
							DestOffset+=RECORD_SIZE;
							return make_ready_future<>();});
						//return make_ready_future<>();//ret;
					}
					else {
						std::cout << "sort_fragments::LoadFragment offs=" << DestOffset << "Empty record" << std::endl;
						return make_ready_future<>();
					}
				}).then([DestFileName, f, RecordsList] () {
					//					return f.dma_write(0,sortResult.data(),FragmentSize).then([rec, DestFileName, f] (size_t s) {
					for(auto&& rec: RecordsList) {
						delete rec;
					}
					return make_ready_future<>();
				});
			}
			else {
				return make_ready_future<>();
			}
		});
	});
}


