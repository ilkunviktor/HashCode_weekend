#include <chrono>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <cmath>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <numeric>
#include <filesystem>
#include <functional>
#include <iterator>
#include <queue>

#define ptr		std::shared_ptr
#define make	std::make_shared

using namespace std;

using uint = uint64_t;

template<typename T>
class CloneFunctor {
public:
	ptr<T> operator()(ptr<T> elem)
	{
		ptr<T> elemNew = make<T>();
		memcpy(elemNew.get(), elem.get(), sizeof(T));

		return elemNew;
	}
};

int main()
{
	string inPath = "../../in";
	string outPath = "../../out";
	uint64_t scoreTotal = 0;
	uint64_t millisecondsTotal = 0;

	for (const auto& taskPath : experimental::filesystem::v1::directory_iterator(inPath))
	{
		chrono::time_point<std::chrono::system_clock> timeStart =
			chrono::system_clock::now();

		stringstream ss;
		ss << taskPath;
		string filePathIn = ss.str();

		size_t fileNameStart = filePathIn.find_last_of('\\');
		size_t fileNameEnd = filePathIn.find_last_of('.');
		string fileName = filePathIn.substr(fileNameStart + 1, fileNameEnd - fileNameStart - 1);

		string filePathOut = outPath + "/" + fileName + ".out";

		// input
		ifstream fileIn;
		fileIn.open(filePathIn);
		assert(fileIn.is_open());

		//fileIn >>

		fileIn.close();

		// solve

		/*
		struct sss
		{
			uint a = 1;
		};

		ptr<sss> s1 = make_shared<sss>();
		ptr<sss> s2 = make_shared<sss>();
		s2->a = 2;
		vector<ptr<sss>> v1;
		v1.push_back(s1);
		v1.push_back(s2);
		vector<ptr<sss>> v2;
		transform(v1.begin(), v1.end(), back_inserter(v2), CloneFunctor<sss>());
		v2[0]->a = 3;
		*/


		// output
		ofstream fileOut;
		fileOut.open(filePathOut);
		assert(fileOut.is_open());

		// fileOut <<  << endl;

		fileOut.close();

		// score
		uint64_t score = 0;
		scoreTotal += score;
		// timing
		chrono::time_point<std::chrono::system_clock> timeEnd =
			chrono::system_clock::now();
		chrono::milliseconds millisec = chrono::duration_cast<chrono::milliseconds>(timeEnd - timeStart);
		millisecondsTotal += millisec.count();
		cout << "file: " << fileName << endl;
		cout << "score: " << score << endl;
		cout << "time(ms): " << to_string(millisec.count()) << endl << endl;
	}

	cout << "scoreTotal: " << scoreTotal << endl;
	cout << "millisecondsTotal: " << millisecondsTotal << endl << endl;
	system("pause");

	return 0;
}