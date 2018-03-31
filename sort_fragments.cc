#include "sort_fragments.hh"
#include "test_fragments.hh"
#include "fragment.hh"

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

					//name = FragmentsList.front();
					return do_for_each(FragmentsList, [this] (sstring name) {
						fragment *Frag = new fragment(name, 0, FragmentSize, SrcFileName);
						std::cout << "sort_fragments::CreateFragments Before Load " << name  << std::endl;
						return Frag->LoadFragment().then([this, Frag] () {
							delete Frag;
							return make_ready_future<>();
						});
					}).then ( [this] () {
					return make_ready_future<std::vector<sstring>>(FragmentsList);
				});
				});
			});
		});
	});
}

