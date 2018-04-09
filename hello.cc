#include "core/app-template.hh"
#include "core/reactor.hh"
#include <iostream>
#include "str_record.hh"
#include "test_fragments.hh"
#include "merge_files.hh"
#include "sort_fragments.hh"

int main(int argc, char** argv) {
    seastar::app_template app;
	 return app.run(argc, argv, [] {
		 test_fragments *pTest = new test_fragments(); // Unit test that creates files
		 return pTest->CreateFiles(5, RECORD_SIZE*4).then([pTest] (std::vector<sstring> results) { // Creates different files for testing,
																																 // I use only the first file
																																 // The parameter to the function is number of test files and size of every file
																																 // I used this code also to test read write on my pc
																																 // If needed, I can pass any of as command line parameters application arguments
																																 // Please note I used hard coded /tmp directory
																																 // All parameters - source, destination directories, size of fragment, memory limitation
																																 // Should be configurable, in real life I would pass json configuration file as program input argument
			 sort_fragments *SortF = new sort_fragments();
			 return SortF->CreateFragments(RECORD_SIZE*2, results[0], "/tmp").then ([results] (std::vector<sstring> fragments) { // This is step 1 of the sorting - fragmentation
																																										// If previous step created more files, I am using only the first created file
																																										// The first parameter is size of fragment, second is input file
																																										// If you like to test with another input, you can pass absolute name second parameter (instead of results[0]) to this method

				 //delete pTest;
				 std::cout << "Hello world " << fragments.size() << std::endl;
				 merge_files *mf = new merge_files();
				 return mf->Process(fragments, RECORD_SIZE).then([garbage = std::move(mf)] () {       // Step 2 : merge fragments and create single output file

					 std::cout << "The end " << std::endl;
					 delete garbage;
					 return make_ready_future<>();
				 });

			 });
		 });
	 });
}



