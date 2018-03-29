#include "sort_fragments.hh"
#include "test_fragments.hh"
#include "str_record.hh"
#include "core/sstring.hh"

sort_fragments::sort_fragments()
{

}


future<std::vector<sstring>> sort_fragments::CreateFragments(size_t FragmentSize, sstring SrcFileName, sstring outDirName)
{
	sstring name = outDirName+"/frag1.bin";
	return LoadFragment(FragmentSize, SrcFileName, 0, name).then([name] () {
		std::vector<sstring> FileNames;
		FileNames.emplace_back(name);
		return make_ready_future<std::vector<sstring>>(FileNames);
	});
}

future<> sort_fragments::LoadFragment(size_t FragmentSize, sstring SrcFileName, size_t SrcOffset, sstring DestFileName)
{
	str_record *rec = new str_record(SrcFileName);
	rec->SetReadFilePos(SrcOffset);

	std::cout << "sort_fragments::LoadFragment --->" << DestFileName << SrcFileName << std::endl;
	return rec->LoadRecord(FragmentSize).then( [FragmentSize, rec, DestFileName] () {
		return open_file_dma(DestFileName,
									open_flags::rw | open_flags::create | open_flags::truncate).then([FragmentSize,rec, DestFileName] (file f) {
			std::cout << "sort_fragments::LoadFragment" << DestFileName << " open done" << std::endl;
			const temporary_buffer<char> &Str = rec->GetStr();
			if(!Str.empty()) {

				//std::sort(Str.begin(), Str.end());

				sstring sortResult = to_sstring(Str.get());
				std::sort(sortResult.begin(), sortResult.end()); // There is obsolete memory copy after sorting and before
																				 // because of aligned temporary buffers!
																				 // But when writing sstring I get exception

				std::unique_ptr<char[], free_deleter> tmp;
				tmp = allocate_aligned_buffer<char>(FragmentSize, IO_BUFFER_ALIGN);
				char *Data = tmp.get();
				memcpy(Data, sortResult.data(), FragmentSize);
				return f.dma_write(0,Data,FragmentSize).then([rec, DestFileName, f] (size_t s) {
					std::cout << "sort_fragments::LoadFragment" << DestFileName << "delete rec, write result=" << s << std::endl;

//					return f.dma_write(0,sortResult.data(),FragmentSize).then([rec, DestFileName, f] (size_t s) {
					delete rec;
					return make_ready_future<>();
				});
			}
			else {
				delete rec;
				return make_ready_future<>();
			}
		});
	});
}


