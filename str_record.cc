#include "str_record.hh"


#define IO_BUFFER_ALIGN 4096

str_record::str_record(const sstring &fname) :
	FileName(fname),  ReadFilePos(0), ReadFileSize(0)
{}

/*
str_record::str_record(const str_record &other)
{
	Copy(other);
}

str_record::str_record()
{}

const str_record &str_record::operator =(const str_record &other)
{
	Copy(other);
	return *this;
}

void str_record::Copy(const str_record &other)
{
	FileName = other.FileName;
	ReadFilePos = other.ReadFilePos;
	Str = other.Str;
}
*/
bool str_record::compare(const str_record &other) const
{
	std::cout << "str_record::compare --->" << std::endl;
	size_t size1 = Str.size();
	size_t size2 = other.Str.size();
	std::cout << "str_record::compare size1=" << size1 << " size2=" << size2 << std::endl;
	int ret = memcmp(Str.get(), other.Str.get(), RECORD_SIZE);
	std::cout << "str_record::compare result" << ret << std::endl;
	if(ret<0)
		return true;
	return false;
}

future<bool> str_record::IsLoadAvailable(size_t size)
{
	if(ReadFileSize>0) {
		if(ReadFilePos+size>ReadFileSize) {
			return make_ready_future<bool>(false);
		}
		return make_ready_future<bool>(true);
	}
	else {
		return open_file_dma(FileName, open_flags::ro).then([this, size] (file f) {
			return do_with(std::move(f), [this, size](file& f) {
				return f.size().then([&f, this, size](uint64_t FileSize) {
					ReadFileSize = FileSize;
					if(ReadFilePos+size>ReadFileSize) {
						return make_ready_future<bool>(false);
					}
					return make_ready_future<bool>(true);
				});
			}).finally([&f]() {
				std::cout << "str_record::IsLoadAvailable finally" << std::endl;
				return f.close();
			});
		});
	}
}

future<> str_record::LoadRecord(size_t size)
{
	return open_file_dma(FileName, open_flags::ro).then([size, this] (file f) {
		std::cout << "str_record::LoadRecord size=" << size << " offset " << ReadFilePos << " " << FileName << std::endl;

		return do_with(std::move(f), [this, size](file& f) {
			std::cout << "str_record::LoadRecord before read bulk" << std::endl;

				future<temporary_buffer<char>> ReadResult = f.dma_read_bulk<char>(ReadFilePos,size);
				std::cout << "str_record::LoadRecord before read bulk then" << std::endl;

				return ReadResult.then([f, this, size] (temporary_buffer<char> buff) {
					std::cout << "str_record::LoadRecord after read bulk" << std::endl;
					if(!buff.empty()) {
						std::cout << "str_record::LoadRecord size1=" << size << " read size=" << buff.size() << std::endl;
						Str = std::move(buff);
					}
					else {
						std::cout << "str_record::LoadRecord ERROR Empty buffer!"<< std::endl;
						Str = temporary_buffer<char>();
					}
					ReadFilePos+=size;
					return make_ready_future<>();
			}).finally([&f, this]() {
				std::cout << "str_record::LoadRecord finally offs "<< ReadFilePos << std::endl;
				return f.close();
			});
			std::cout << "str_record::LoadRecord 1"<< std::endl;
		});
		std::cout << "str_record::LoadRecord 2"<< std::endl;
	}).handle_exception([this](std::exception_ptr ep) -> future<> {
		std::cout << "str_record::LoadRecord Cannot read" << FileName << " offset" << ReadFilePos << std::endl;
		try {
			std::rethrow_exception(std::move(ep));
		} catch (...) {
			std::throw_with_nested(std::runtime_error(sstring("Could not read ") +  FileName));
		}
	});
}

