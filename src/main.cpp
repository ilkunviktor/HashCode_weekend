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
#include <unordered_map>

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

	struct Factors
	{
		float f1 = 0.f;
		float f2 = 0.f;
		float f3 = 0.f;
		float f4 = 0.f;
	};

	map<string, Factors> factorsGlobal;// = { {"dc", {0.49f, 0.51f, 0.f, 0.1f}} };

	for (const auto& taskPath : experimental::filesystem::v1::directory_iterator(inPath))
	{
		uint millisecondsFile = 0;
		stringstream ss;
		ss << taskPath;
		string filePathIn = ss.str();

		size_t fileNameStart = filePathIn.find_last_of('\\');
		size_t fileNameEnd = filePathIn.find_last_of('.');
		string fileName = filePathIn.substr(fileNameStart + 1, fileNameEnd - fileNameStart - 1);

		string filePathOut = outPath + "/" + fileName + ".out";

		uint scoreBest = 0;
		Factors factorsBest;
		float factorStep = 0.01f;

		for (float factorCur = 0.f; factorCur < 1.f; factorCur += factorStep)
		{
			chrono::time_point<std::chrono::system_clock> timeStart =
				chrono::system_clock::now();

			Factors factors;

			auto foundFileFactorsIt = factorsGlobal.find(fileName);

			if (foundFileFactorsIt != factorsGlobal.end())
			{
				factorStep += 1.f;
				factors = foundFileFactorsIt->second;
			}
			else
			{
				factors.f1 = factorCur;
				factors.f2 = 1.f - factors.f1;
				//factors.f3 = 0.f;
				//factors.f4 = 1.f;
			}


			// comment this for factors iter!
			factorStep += 1.f;
			factors.f1 = 0.5f;
			factors.f2 = 0.5f;



			// input
			ifstream fileIn;
			fileIn.open(filePathIn);
			assert(fileIn.is_open());

			uint R = 0;
			uint C = 0;
			uint L = 0;
			uint H = 0;
			fileIn >> R >> C >> L >> H;

			struct Cell
			{
				uint r = 0;
				uint c = 0;
				char t = '\0';
			};

			vector<vector<char>> pizza;

			for (uint i = 0; i < R; ++i)
			{
				vector<char> row;

				for (uint j = 0; j < C; ++j)
				{
					Cell c;
					char el = '\0';
					fileIn >> el;
					row.emplace_back(el);
				}

				pizza.emplace_back(row);
			}
			
			fileIn.close();

			// solve
			


			// score

			uint score = 0;



			if (score > scoreBest)
			{
				scoreBest = score;
				factorsBest.f1 = factors.f1;
				factorsBest.f2 = factors.f2;
				factorsBest.f3 = factors.f3;
				factorsBest.f4 = factors.f4;

				// output
				ofstream fileOut;
				fileOut.open(filePathOut);
				assert(fileOut.is_open());

				//fileOut <<
				
				fileOut.close();
			}

			// timing
			chrono::time_point<std::chrono::system_clock> timeEnd =
				chrono::system_clock::now();
			chrono::milliseconds millisec = chrono::duration_cast<chrono::milliseconds>(timeEnd - timeStart);
			uint millisecondsPart = millisec.count();
			millisecondsFile += millisecondsPart;
			cout << "file: " << fileName << endl;
			cout << "score: " << score << endl;
			cout << "factors.f1: " << factors.f1 << endl;
			cout << "factors.f2: " << factors.f2 << endl;
			cout << "factors.f3: " << factors.f3 << endl;
			cout << "factors.f4: " << factors.f4 << endl;
			cout << "time part file(ms): " << to_string(millisecondsPart) << endl << endl;
		}

		scoreTotal += scoreBest;
		millisecondsTotal += millisecondsFile;
		cout << "scoreBest: " << scoreBest << endl;
		cout << "factorsBest.f1: " << factorsBest.f1 << endl;
		cout << "factorsBest.f2: " << factorsBest.f2 << endl;
		cout << "factorsBest.f3: " << factorsBest.f3 << endl;
		cout << "factorsBest.f4: " << factorsBest.f4 << endl;
		cout << "time file(ms): " << millisecondsTotal << endl << endl;
	}

	cout << "scoreTotal: " << scoreTotal << endl;
	cout << "time total(ms): " << millisecondsTotal << endl << endl;
	system("pause");

	return 0;
}