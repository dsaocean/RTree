#include "CppUnitTest.h"

#include "../RTree.h"

#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct Rect
{
	Rect(double a_minX, double a_minY, double a_maxX, double a_maxY)
		: id(Rect::max_id++)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}

	static size_t max_id;
	size_t id;
	double min[2];
	double max[2];
};
size_t Rect::max_id = 0;

namespace UnitTests
{
	TEST_CLASS(UnitTests)
	{
	public:


		std::array<Rect,5> rects
		{
		  // xmin, ymin, xmax, ymax
		  Rect(0.0, 0.0, 2.0, 1.0), 
		  Rect(5.0, 5.0, 7.0, 7.0),
		  Rect(8.0, 5.0, 9.0, 6.0),
		  Rect(7.0, 1.0, 9.0, 2.0),
		  Rect(4.0, 5.0, 5.0, 6.0),
		};

		/// <summary>
		/// Tests that a nearest neighbor works with a flat tree
		/// </summary>
		/// <remarks>
		/// maxNodes is larger than the number of rects.  This
		/// ensures that the root node can fit all the entries.
		/// Therefore, the tree only has a single layer
		/// </remarks>
		TEST_METHOD(FlatTree)
		{
			int constexpr maxNodes = 8;
			RTree<Rect*, double, 2, double, maxNodes> tree;

			for (auto& r : rects)
			{
				tree.Insert(r.min, r.max, &r);
			}

			std::vector<size_t> order;

			double x_min[2] = { 4.0, 3.0 };
			double x_max[2] = { 4.0, 3.0 };

			auto count = tree.NNSearch(x_min, x_max,
				[&order](Rect* r, double distance)
				{
					order.push_back(r->id);
					return true;
				}
			);

			Assert::AreEqual(rects.size(), count);
			Assert::AreEqual(rects[4].id, order[0]);
			Assert::AreEqual(rects[1].id, order[1]);
			Assert::AreEqual(rects[0].id, order[2]);
			Assert::AreEqual(rects[3].id, order[3]);
			Assert::AreEqual(rects[2].id, order[4]);
		}

		/// <summary>
		/// Tests that a nearest neighbor works with a deep tree
		/// </summary>
		/// <remarks>
		/// maxNodes is smaller than the number of rects.  This
		/// ensures that the root node cannot fit all the entries.
		/// Therefore, the tree must have multiple layers.
		/// </remarks>
		TEST_METHOD(DeepTree)
		{
			int constexpr maxNodes = 2;
			RTree<Rect*, double, 2, double, maxNodes> tree;

			for (auto & r : rects)
			{
				tree.Insert(r.min, r.max, &r);
			}

			std::vector<size_t> order;

			double x_min[2] = { 4.0, 3.0 };
			double x_max[2] = { 4.0, 3.0 };

			auto count = tree.NNSearch(x_min, x_max,
				[&order](Rect* r, double distance)
				{
					order.push_back(r->id);
					return true;
				}
			);

			Assert::AreEqual(rects.size(), count);
			Assert::AreEqual(rects[4].id, order[0]);
			Assert::AreEqual(rects[1].id, order[1]);
			Assert::AreEqual(rects[0].id, order[2]);
			Assert::AreEqual(rects[3].id, order[3]);
			Assert::AreEqual(rects[2].id, order[4]);
		}

		/// <summary>
		/// Verifies that the search can be stopped after a
		/// fixed number of results are found
		/// </summary>
		TEST_METHOD(MaxNumberOfHits)
		{
			RTree<Rect*, double, 2, double, 3> tree;
			size_t constexpr maxHits = 2;

			for (auto& r : rects)
			{
				tree.Insert(r.min, r.max, &r);
			}

			std::vector<size_t> order;

			double x_min[2] = { 4.0, 3.0 };
			double x_max[2] = { 4.0, 3.0 };

			auto count = tree.NNSearch(x_min, x_max,
				[&order, maxHits](Rect* r, double distance)
				{
					order.push_back(r->id);
					return order.size() < maxHits;
				}
			);

			Assert::AreEqual(size_t(maxHits), count);
		}
	};
}
