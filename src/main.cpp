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

	map<string, Factors> factorsGlobal = { {"dc", {0.49f, 0.51f, 0.f, 0.f}} };

	for (const auto& taskPath : experimental::filesystem::v1::directory_iterator(inPath))
	{
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
			Factors factors;

			auto foundFileFactorsIt = factorsGlobal.find(fileName);

			if (foundFileFactorsIt != factorsGlobal.end())
			{
				//factorStep += 1.f;
				factors = foundFileFactorsIt->second;


				//factors.f1 = factorCur;
				//factors.f2 = 1.f - factors.f1;
				factors.f3 = factorCur;
				factors.f4 = 1.f - factors.f3;
			}
			else
			{
				factors.f1 = factorCur;
				factors.f2 = 1.f - factors.f1;
				//factors.f3 = factorCur;
				//factors.f4 = 1.f - factors.f3;
			}

			chrono::time_point<std::chrono::system_clock> timeStart =
				chrono::system_clock::now();

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

			enum class SlotType
			{
				Free,
				Full
			};

			struct Server
			{
				uint id = 0;
				uint slotsCount = 0;
				uint capacity = 0;
				bool placed = false;
				Point point;
				uint poolId = 0;

				float capacityPerSlot = 0.f;
				vector<SlotType> slots;
			};

			vector<ptr<Server>> servers;
			uint slotsCountAll = 0;

			for (uint i = 0; i < serversCount; ++i)
			{
				ptr<Server> server = make<Server>();
				server->id = i;
				fileIn >> server->slotsCount >> server->capacity;
				slotsCountAll += server->slotsCount;
				server->capacityPerSlot = (float)server->capacity / (float)server->slotsCount;
				server->slots.resize(server->slotsCount, SlotType::Free);
				servers.emplace_back(server);
			}

			uint freeSlots = rowsCount * slotsCount - unSlotsCount;

			fileIn.close();

			// solve
			struct Row
			{
				uint id = 0;
				vector<SlotType> slots;
				uint filledCount = 0;
				uint capacityTotal = 0;
				uint emptySlotsRowMax = 0;
				deque<ptr<Server>> serversPlaced;
			};

			vector<ptr<Row>> rows; // key = row id

			for (uint i = 0; i < rowsCount; ++i)
			{
				ptr<Row> row = make<Row>();
				row->id = i;
				row->slots.resize(slotsCount, SlotType::Free);
				rows.emplace_back(row);
			}

			for (auto&& unslot : unSlots)
			{
				rows[unslot->point.row]->slots[unslot->point.col] = SlotType::Full;
				rows[unslot->point.row]->filledCount += 1;
			}

			for (auto&& rowP : rows)
			{
				uint emptySlotsRowMax = 0;
				rowP->emptySlotsRowMax = 0;

				for (auto&& slot : rowP->slots)
				{
					if (slot == SlotType::Free)
					{
						++emptySlotsRowMax;

						if (emptySlotsRowMax > rowP->emptySlotsRowMax)
						{
							rowP->emptySlotsRowMax = emptySlotsRowMax;
						}
					}
					else
					{
						if (emptySlotsRowMax > rowP->emptySlotsRowMax)
						{
							rowP->emptySlotsRowMax = emptySlotsRowMax;
						}

						emptySlotsRowMax = 0;
					}
				}
			}

			auto sort_row = [&factors](const ptr<Row>& r1, const ptr<Row>& r2)
			{
				float filledFactor = (float)r1->filledCount / (float)(r1->filledCount + r2->filledCount);
				float emptySlotsRowMaxFactor = (float)r1->emptySlotsRowMax / (float)(r1->emptySlotsRowMax + r2->emptySlotsRowMax);
				uint capacityTotal = r1->capacityTotal + r2->capacityTotal;

				if (capacityTotal > 0)
				{
					float capacityFactor = (float)r1->capacityTotal / (float)(capacityTotal);
					return factors.f1 * filledFactor + factors.f2 * capacityFactor < 0.5f;
				}
				else
				{
					return filledFactor < 0.5f;
				}
			};

			vector<ptr<Row>> rowsP;
			copy(rows.begin(), rows.end(), back_inserter(rowsP));
			sort(rowsP.begin(), rowsP.end(), sort_row);

			deque<ptr<Server>> serversP;
			copy(servers.begin(), servers.end(), back_inserter(serversP));
			sort(serversP.begin(), serversP.end(), [&factors](const ptr<Server>& s1, const ptr<Server>& s2)
			{
				float sizeFactor = (float)s1->slotsCount / (float)(s1->slotsCount + s2->slotsCount);
				float capacityFactor = (float)s1->capacity / (float)(s1->capacity + s2->capacity);
				float capacityPerSlotFactor = (float)s2->capacityPerSlot / (float)(s1->capacityPerSlot + s2->capacityPerSlot);
				return sizeFactor * factors.f3 + capacityPerSlotFactor * factors.f4 < 0.5f;
			});

			uint rowPId = 0;

			while (!serversP.empty())
			{
				sort(rowsP.begin(), rowsP.end(), sort_row);
				ptr<Server> serverP = *serversP.begin();

				for (auto&& rowP : rowsP)
				{
					auto foundIt = search(rowP->slots.begin(), rowP->slots.end(),
						serverP->slots.begin(), serverP->slots.end());

					if (foundIt != rowP->slots.end())
					{
						serverP->placed = true;
						serverP->point.row = rowP->id;
						serverP->point.col = distance(rowP->slots.begin(), foundIt);

						rowP->filledCount += serverP->slotsCount;
						rowP->capacityTotal += serverP->capacity;
						rowP->serversPlaced.push_back(serverP);

						for (auto it = foundIt; it != foundIt + serverP->slots.size(); ++it)
						{
							*it = SlotType::Full;
						}

						uint emptySlotsRowMax = 0;
						rowP->emptySlotsRowMax = 0;

						for (auto&& slot : rowP->slots)
						{
							if (slot == SlotType::Free)
							{
								++emptySlotsRowMax;

								if (emptySlotsRowMax > rowP->emptySlotsRowMax)
								{
									rowP->emptySlotsRowMax = emptySlotsRowMax;
								}
							}
							else
							{
								if (emptySlotsRowMax > rowP->emptySlotsRowMax)
								{
									rowP->emptySlotsRowMax = emptySlotsRowMax;
								}

								emptySlotsRowMax = 0;
							}
						}

						break;
					}
				}

				serversP.pop_front();
			}
			
			// fill pools

			struct Pool
			{
				uint id = 0;
				map<uint, vector<ptr<Server>>> serversInRows; // key = row id
				uint gc = 0;
				uint capacityTotal = 0;

				void CalcGC(uint rowsCount)
				{
					uint gcMin = UINT_MAX;

					for (uint rowBad = 0; rowBad < rowsCount; ++rowBad)
					{
						uint gc = 0;

						for (auto&& serversInRow : serversInRows)
						{
							if (serversInRow.first == rowBad)
							{
								continue;
							}
							else
							{
								for (auto&& server : serversInRow.second)
								{
									gc += server->capacity;
								}
							}
						}

						if (gc < gcMin)
						{
							gcMin = gc;

							if (gcMin == 0)
							{
								break;
							}
						}
					}

					gc = gcMin;
				}
			};

			vector<ptr<Pool>> pools;

			for (uint i = 0; i < poolsCount; ++i)
			{
				ptr<Pool> pool = make<Pool>();
				pool->id = i;
				pools.emplace_back(pool);
			}

			auto sort_pool = [](const ptr<Pool>& p1, const ptr<Pool>& p2)
			{
				return p1->gc < p2->gc;
			};

			sort(pools.begin(), pools.end(), sort_pool);

			bool again = true;

			while (again)
			{
				again = false;
				sort(rowsP.begin(), rowsP.end(), sort_row);

				for (auto rowPIt = rowsP.rbegin(); rowPIt != rowsP.rend(); ++rowPIt)
				{
					ptr<Row> rowP = *rowPIt;

					for (auto&& server : rowP->serversPlaced)
					{
						ptr<Pool> pool = *pools.begin();

						auto foundIt = pool->serversInRows.find(rowP->id);

						if (foundIt == pool->serversInRows.end())
						{
							vector<ptr<Server>> serversInRows;
							serversInRows.push_back(server);
							server->poolId = pool->id;
							pool->serversInRows[rowP->id] = serversInRows;
						}
						else
						{
							server->poolId = pool->id;
							foundIt->second.push_back(server);
						}


						pool->CalcGC(rowsCount);

						sort(pools.begin(), pools.end(), sort_pool);

						rowP->serversPlaced.pop_front();

						rowP->filledCount -= server->slotsCount;
						rowP->capacityTotal -= server->capacity;

						again = true;
						break;
					}
				}
			}

			// score

			uint gcMinTotal = UINT_MAX;

			for (auto&& pool : pools)
			{
				if (pool->gc < gcMinTotal)
				{
					gcMinTotal = pool->gc;
				}
			}

			uint score = gcMinTotal;
			// timing
			chrono::time_point<std::chrono::system_clock> timeEnd =
				chrono::system_clock::now();
			chrono::milliseconds millisec = chrono::duration_cast<chrono::milliseconds>(timeEnd - timeStart);
			millisecondsTotal += millisec.count();
			cout << "file: " << fileName << endl;
			cout << "score: " << score << endl;
			cout << "time(ms): " << to_string(millisec.count()) << endl << endl;

			if (score > scoreBest)
			{
				scoreBest = score;
				scoreTotal = scoreBest;
				factorsBest.f1 = factors.f1;
				factorsBest.f2 = factors.f2;
				factorsBest.f3 = factors.f3;
				factorsBest.f4 = factors.f4;

				// output
				ofstream fileOut;
				fileOut.open(filePathOut);
				assert(fileOut.is_open());

				for (const auto& server : servers)
				{
					if (server->placed)
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
			}
		}

		cout << "scoreBest: " << scoreBest << endl;
		cout << "factorsBest.f1: " << factorsBest.f1 << endl;
		cout << "factorsBest.f2: " << factorsBest.f2 << endl;
		cout << "factorsBest.f3: " << factorsBest.f3 << endl;
		cout << "factorsBest.f4: " << factorsBest.f4 << endl;
		cout << "millisecondsTotal: " << millisecondsTotal << endl << endl;
	}

	//cout << "scoreTotal: " << scoreTotal << endl;
	//cout << "millisecondsTotal: " << millisecondsTotal << endl << endl;
	system("pause");

	return 0;
}