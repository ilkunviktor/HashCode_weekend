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
		
		uint rowsCount = 0;
		uint slotsCount = 0;
		uint unSlotsCount = 0;
		uint poolsCount = 0;
		uint serversCount = 0;

		fileIn >> rowsCount >> slotsCount >> unSlotsCount >> poolsCount >> serversCount;
		
		struct Point
		{
			uint row = 0;
			uint col = 0;
		};

		struct UnSlot
		{
			uint id = 0;
			Point point;
		};

		vector<ptr<UnSlot>> unSlots;

		for (uint i = 0; i < unSlotsCount; ++i)
		{
			ptr<UnSlot> unSlot = make<UnSlot>();
			unSlot->id = i;
			fileIn >> unSlot->point.row >> unSlot->point.col;
			unSlots.emplace_back(unSlot);
		}

		struct Server
		{
			uint id = 0;
			uint slotsCount = 0;
			uint capacity = 0;
			bool allocated = false;
			Point point;
			uint poolId = 0;
		};

		vector<ptr<Server>> servers;

		for (uint i = 0; i < serversCount; ++i)
		{
			ptr<Server> server = make<Server>();
			server->id = i;
			fileIn >> server->slotsCount >> server->capacity;
			servers.emplace_back(server);
		}

		fileIn.close();

		// solve
		struct Row
		{
			uint id = 0;
			set<uint> bad;
		};

		vector<ptr<Row>> rows; // key = row id
	
		for (uint i = 0; i < rowsCount; ++i)
		{
			ptr<Row> row = make<Row>();
			row->id = i;
			rows.emplace_back(row);
		}

		for (auto&& unslot : unSlots)
		{
			rows[unslot->point.row]->bad.insert(unslot->point.col);
		}

		vector<ptr<Server>> rowsP;
		copy(rows.begin(), rows.end(), back_inserter(rowsP));
		sort(rowsP.begin(), rowsP.end(), [](const ptr<Row>& r1, const ptr<Row>& r2)
		{
			r1->bad.size() > r1->bad.size();
		});

		/*struct Pool
		{
			vector<ptr<Server>> serverPlaced;
		};*/

		vector<ptr<Server>> serversP;
		copy(servers.begin(), servers.end(), back_inserter(serversP));

		sort(serversP.begin(), serversP.end(), [](const ptr<Server>& s1, const ptr<Server>& s2)
		{
			s1->slotsCount < s2->slotsCount;
		});

		for (auto&& rowP : rowsP)
		{
			for(uint i = 0; i < poolsCount; ++i)
		}

		while (!serversP.empty())
		{

		}

		// output
		ofstream fileOut;
		fileOut.open(filePathOut);
		assert(fileOut.is_open());

		for (const auto& server : servers)
		{
			if (server->allocated)
			{
				fileOut << to_string(server->point.row) << " " << 
					to_string(server->point.col) << " " << 
					to_string(server->poolId) << endl;
			}
			else
			{
				fileOut << "x" << endl;
			}
		}

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