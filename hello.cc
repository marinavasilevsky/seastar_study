#include "core/app-template.hh"
#include "core/reactor.hh"
#include <iostream>
#include "str_record.h"
#include "test_fragments.h"

int main(int argc, char** argv) {
    seastar::app_template app;
	 return app.run(argc, argv, [] {
		 test_fragments *pTest = new test_fragments();
		 return pTest->CreateFiles(5, 4096).then([pTest] (std::vector<sstring> results) {
			 //delete pTest;
			 std::cout << "Hello world";
			 return make_ready_future<>();
		 });
#if 0
    });
#endif
});
}



/*
 *
 *
#include <vector>
#include <boost/range/algorithm.hpp>

std::vector<std::string> stringarray;
boost::sort(stringarray);
 *
 */

#if 0
#include <iostream>
#include <queue>
using namespace std;

class Record
{
private:
	 std::string FileName;
	 std::string FirstRecord;

public:
	 Record( std::string FileName,
				std::string FirstRecord, )  : value( v ) {}
	 int Val() const { return value; }
};

struct CompareNode : public std::binary_function<Node*, Node*, bool>
{
  bool operator()(const Node* lhs, const Node* rhs) const
  {
	  return lhs->Val() < rhs->Val();
  }
};

int main()
{
	 priority_queue<Node*,vector<Node*>, CompareNode > pq;

	 pq.push( new Node( 111 ) );
	 pq.push( new Node( 1111 ) );
	 pq.push( new Node( 1011 ) );
	 pq.push( new Node( 100 ) );
	 pq.push( new Node( 1110 ) );
	 pq.push( new Node( 101 ) );

	 while ( !pq.empty() )
	 {
		  Node* n = pq.top();
		  cout << n->Val() << endl;
		  pq.pop();

		  // Delete pointer that vector contains
		  delete n;
	 }
	 cin.get();
}

template <typename PriorityQueue>
void basic_interface(void)
{
	 PriorityQueue pq;

	 pq.push(2);
	 pq.push(3);
	 pq.push(1);

	 cout << "Priority Queue: popped elements" << endl;
	 cout << pq.top() << " "; // 3
	 pq.pop();
	 cout << pq.top() << " "; // 2
	 pq.pop();
	 cout << pq.top() << " "; // 1
	 pq.pop();
	 cout << endl;
}
#endif
