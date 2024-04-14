#include "pch.h"
#include "CppUnitTest.h"
#include "Chunk.h"
#include <vector>

using namespace fefu_laboratory_two;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ChunkListUnitTest
{
	TEST_CLASS(ConstructorTests)
	{
	public:
		TEST_METHOD(DefaultConstructorTest)
		{
			ChunkList<int, 8> list;
			Assert::IsTrue(list.empty() == true);
			Assert::IsTrue(list.size() == 0);
		}

		TEST_METHOD(FromChunkListConstructor)
		{
			ChunkList<int, 8> list;
			for (int i = 0; i < 8; i++) {
				list.push_back(i);
			}
			auto list2 = list;
			Assert::IsTrue(list == list2);
		}

		TEST_METHOD(FromInitListConstructor)
		{
			ChunkList<int, 4> list;
			for (int i = 0; i < 4; i++) {
				list.push_back(i);
			}
			ChunkList<int, 4> list2 = { 0, 1, 2, 3 };
			Assert::IsTrue(list == list2);
		}

		TEST_METHOD(FromRangeConstructor)
		{
			ChunkList<int, 4> list;
			std::vector<int> range;
			for (int i = 0; i < 4; i++) {
				list.push_back(i);
				range.push_back(i);
			}
			ChunkList<int, 4> list2(range.begin(), range.end());
			Assert::IsTrue(list == list2);
		}

		TEST_METHOD(Assign)
		{
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;
			for (int i = 0; i < 8; i++) {
				list.push_back(2);
			}
			list2.assign(8, 2);

			Assert::IsTrue(list == list2);
		}

		TEST_METHOD(AssignIt)
		{
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;
			std::vector<int> v;
			for (int i = 0; i < 8; i++) {
				list.push_back(2);
				v.push_back(2);
			}
			list2.assignIt(v.begin(), v.end());

			Assert::IsTrue(list == list2);
		}

		TEST_METHOD(AssignFromInitList)
		{
			ChunkList<int, 4> list;
			ChunkList<int, 4> list2;
			for (int i = 0; i < 4; i++) {
				list.push_back(i);
			}
			list2.assign({ 0, 1, 2, 3 });

			Assert::IsTrue(list == list2);
		}
	};

	TEST_CLASS(COMPARISIONSTests)
	{
	public:
		TEST_METHOD(SpaceshipOperator)
		{
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;
			for (int i = 0; i < 8; i++) {
				list.push_back(2);
				list2.push_back(2);
			}

			auto res = list <=> list2;
			Assert::IsTrue(res == std::strong_ordering::equal);
		}

		TEST_METHOD(EquelOperator)
		{
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;
			for (int i = 0; i < 8; i++) {
				list.push_back(2);
				list2.push_back(2);
			}

			auto res = list == list2;
			Assert::IsTrue(res);
		}

		TEST_METHOD(NotEquelOperator)
		{
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;
			for (int i = 0; i < 8; i++) {
				list.push_back(2);
				list2.push_back(3);
			}

			auto res = list != list2;
			Assert::IsTrue(res);
		}

		TEST_METHOD(LessOperator)
		{
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;
			for (int i = 0; i < 8; i++) {
				list.push_back(2);
				list2.push_back(3);
			}

			auto res = list < list2;
			Assert::IsTrue(res);
		}

		TEST_METHOD(GreaterOperator)
		{
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;
			for (int i = 0; i < 8; i++) {
				list.push_back(3);
				list2.push_back(2);
			}

			auto res = list > list2;
			Assert::IsTrue(res);
		}
	};

	TEST_CLASS(ElementAccessTests)
	{
	public:
		TEST_METHOD(At)
		{
			ChunkList<int, 8> list;
			for (int i = 0; i < 30; i++) {
				list.push_back(i);
			}
			Assert::IsTrue(list.at(0) == 0);
			Assert::IsTrue(list.at(15) == 15);
			Assert::ExpectException<std::out_of_range>([]() {
				ChunkList<int, 8> l;
				l.at(31);
				});
		}

		TEST_METHOD(OperatorBrackets)
		{
			ChunkList<int, 8> list;
			for (int i = 0; i < 30; i++) {
				list.push_back(i);
			}
			Assert::IsTrue(list[0] == 0);
			Assert::IsTrue(list[15] == 15);
		}
	};

	TEST_CLASS(IteratorTests) {
		TEST_METHOD(Iterators) {
			ChunkList<int, 8> list;
			int* arr = new int[15];
			for (int i = 0; i < 15; i++) {
				list.push_back(i);
				arr[i] = i;
			}

			int j = 0;
			for (auto e : list)
				Assert::IsTrue(arr[j++] == e);

			auto it1 = list.begin();
			auto it2 = list.begin();
			Assert::IsTrue(it1 == it2);
			Assert::IsTrue(it1 >= it2);
			it2++;
			Assert::IsTrue(it2 > it1);
			it1 += 3;
			Assert::IsFalse(it1 < it2);
		}
	};

	TEST_CLASS(CapacityTests) {
		TEST_METHOD(CapacityMethods) {
			ChunkList<int, 8> list;
			Assert::IsTrue(list.empty() == true);
			Assert::IsTrue(list.size() == 0);
			Assert::IsTrue(list.max_size() == 0);
			for (int i = 0; i < 9; i++) {
				list.push_back(i);
			}
			Assert::IsTrue(list.size() == 9);
			Assert::IsTrue(list.max_size() == 16);
		}
	};

	TEST_CLASS(MODIFIERSTests) {
		TEST_METHOD(Insert) {
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;
			list2.push_back(100);
			list2.push_back(0);
			list2.push_back(1);
			list2.push_back(2);

			for (int i = 0; i < 10; i++)
				list.push_back(i);

			list.clear();

			Assert::IsTrue(list.empty() == true);
			Assert::IsTrue(list.size() == 0);
			Assert::IsTrue(list.max_size() == 0);

			for (int i = 0; i < 3; i++)
				list.push_back(i);

			auto it = list.cbegin();
			list.insert(it, 100);

			Assert::IsTrue(list == list2);
		}

		TEST_METHOD(Erase) {
			ChunkList<int, 8> list;
			ChunkList<int, 8> list2;

			for (int i = 1; i < 8; i++)
				list2.push_back(i);


			for (int i = 0; i < 8; i++)
				list.push_back(i);

			list.erase(list.cbegin());

			Assert::IsTrue(list == list2);
		}

		TEST_METHOD(PushAndPop) {
			ChunkList<int, 8> list;

			for (int i = 0; i < 8; i++)
				list.push_back(i);

			Assert::IsTrue(list[0] == 0);
			Assert::IsTrue(list[7] == 7);

			list.pop_back();
			Assert::IsTrue(list.size() == 7);

			list.push_front(350);
			Assert::IsTrue(list[0] == 350);

			list.pop_front();
			Assert::IsTrue(list[0] == 0);
		}

		TEST_METHOD(Emplace) {
			ChunkList<int, 8> list;

			for (int i = 0; i < 8; i++)
				list.push_back(i);

			list.emplace_back(10);
			list.emplace_front(22);

			Assert::IsTrue(list[0] == 22);
			Assert::IsTrue(list[9] == 10);
		}
	};

	TEST_CLASS(RestructureTests) {
		TEST_METHOD(Resize) {
			ChunkList<int, 4> list;
			auto list2 = list;

			for (int i = 0; i < 4; i++)
				list.push_back(i);

			list.resize(3);

			list2.push_back(0);
			list2.push_back(1);
			list2.push_back(2);

			Assert::IsTrue(list == list2);
		}
	};

	TEST_CLASS(SwapTests) {
		TEST_METHOD(Swap) {
			ChunkList<int, 4> list;
			auto list2 = list;
			auto list3 = list;

			for (int i = 0; i < 4; i++) {
				list.push_back(1);
				list2.push_back(2);
				list3.push_back(2);
			}

			Assert::IsTrue(list != list2);
			Assert::IsTrue(list2 == list3);

			list.swap(list2);

			Assert::IsTrue(list == list3);
			Assert::IsTrue(list2 != list3);
		}
	};
}
