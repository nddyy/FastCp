#include <iostream>
#include <ctime>
#include "fast_cp.h"
using namespace std;

int main(/*int argc, char** argv*/) {

	size_t buffer_size;
	int thread;

	/*if (argc < 4)
	{
		std::cout << "Error: you should input copy_file_path, result_file_path and the number of threads!\n";
		return -1;
	}*/
	//std::string s1 = argv[1];
	//std::string s2 = argv[2];
	//thread = atoi(argv[3]);

	std::string s1 = "E:\\wts\\FastCp\\test.cal";
	std::string s2 = "E:\\wts\\FastCp\\result.cal";
	thread = 5;
	buffer_size = 1000000;

	int start = clock();
	run_fast_cp(buffer_size, thread, s1, s2);
	printf("\nTotal cost time %6.2fs\n", double(clock() - start) / CLOCKS_PER_SEC);

	system("pause");
	return 0;
}