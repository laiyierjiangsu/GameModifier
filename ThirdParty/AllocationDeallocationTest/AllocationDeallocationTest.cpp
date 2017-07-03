// AllocationDeallocationTest.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "ObjectPool.h"
#include <time.h>

const int NUMBER_OF_ITEMS = 100;
const int NUMBER_OF_ITERATIONS = 1000000;

int _tmain(int argc, _TCHAR* argv[])
{
	// I know. Allocating a char is stupid, but the purpose is to show
	// how the pool is fast to allocate and deallocate small objects.

	ObjectPool<char> pool;
	char* variables[NUMBER_OF_ITEMS];

	std::cout << "This application only tests the performance of the pool versus normal" << std::endl;
	std::cout << "new and delete calls. To get the real results, compile it in release mode" << std::endl;
	std::cout << "and run it outside Visual Studio" << std::endl << std::endl;

	std::cout << "Starting to test the pool..." << std::endl;
	int start = clock();
	for(int j=0;j<NUMBER_OF_ITERATIONS; j++)
	{
		for(int i=0; i<NUMBER_OF_ITEMS; i++)
			variables[i] = pool.New();

		for(int i=0; i<NUMBER_OF_ITEMS; i++)
			pool.Delete(variables[i]);
	}
	int end = clock();

	std::cout << "Time spent using the pool: " << end - start << " milliseconds" << std::endl << std::endl;

	std::cout << "Starting to test the normal new and delete calls..." << std::endl;
	start = clock();
	for(int j=0;j<NUMBER_OF_ITERATIONS; j++)
	{
		for(int i=0; i<NUMBER_OF_ITEMS; i++)
			variables[i] = new char(2);

		for(int i=0; i<NUMBER_OF_ITEMS; i++)
			delete variables[i];
	}
	end = clock();

	std::cout << "Time spent using normal new/delete calls: " << end - start << " milliseconds" << std::endl << std::endl;
	std::cout << "Program finished. Press any key to close." << std::endl;
	std::cin.ignore();

	return 0;
}

