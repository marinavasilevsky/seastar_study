#include "test_fragments.hh"
#include "core/reactor.hh"
#include "core/fstream.hh"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "core/align.hh"

test_fragments::test_fragments()
{

}

future<std::vector<sstring>> test_fragments::CreateFiles(int NumOfFiles, size_t size)
{
	std::vector<future<>> fs;
	std::vector<sstring> FileNames;
	int i;

	std::cout << "test_fragments::CreateFiles --->" << std::endl;
	sstring data;
	for(i=0;i<NumOfFiles;i++) {
		sstring name = sstring("/tmp/test")+to_sstring(i);
		FileNames.emplace_back(name);
		fs.emplace_back(std::move( CreateFragment(name, size)));
	}

	return when_all(fs.begin(), fs.end()).then([FileNames] (std::vector<future<>> results) {
		std::cout << "test_fragments::CreateFiles done" << std::endl;
		return make_ready_future<std::vector<sstring>> (FileNames);
	});
}

future<> test_fragments::CreateFragment(sstring name, size_t size)
{
	sstring str;

	std::cout << "test_fragments::CreateFragment --->" << name << std::endl;
	return open_file_dma(name,
								open_flags::rw | open_flags::create | open_flags::truncate).then([this, size,str,name] (file f) {		
		std::cout << name << "open done" << std::endl;
		return do_with(std::move(f), [size, name](file& f) {

			std::unique_ptr<char[], free_deleter> tmp;
			tmp = allocate_aligned_buffer<char>(size, IO_BUFFER_ALIGN);
			char *Data = tmp.get();
			size_t i;
			int num = 16;
			char RandChar = 16;

			for(i=0;i<size;i++) {
				if((i & 0xfff) == 0) {
					num = rand();
					if(num==(int)'\0')
						num++;
					RandChar = (char)(num & 0xff);
					std::cout << "test_fragments::CreateFragment rand=" << RandChar << std::endl;
				}
				Data[i] = RandChar;
			}

			return f.dma_write(0,Data,size).then([&f, name] (size_t s) {
				std::cout << "test_fragments::CreateFragment " << name << "write result=" << s << std::endl;
				return make_ready_future<>();
			}).finally([&f]() {
				std::cout << "test_fragments::CreateFragment finally" << std::endl;
				return f.close();
			});
		});
	});
}
