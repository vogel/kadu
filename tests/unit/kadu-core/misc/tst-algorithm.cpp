/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "misc/algorithm.h"

#include <vector>
#include <QtTest/QtTest>

class tst_Algorithm : public QObject
{
	Q_OBJECT

private slots:
	void findOverlappingRegionEmptyTest();
	void findOverlappingRegionFirstEmptyTest();
	void findOverlappingRegionSecondEmptyTest();
	void findOverlappingRegionSameTest();
	void findOverlappingRegionRemoveFirstTest();
	void findOverlappingRegionAddOneTest();
	void findOverlappingRegionOverlapingTest();
	void findOverlappingRegionDifferentTest();
	void findOverlappingRegionOverlapingDifferentTest();

};

void tst_Algorithm::findOverlappingRegionEmptyTest()
{
	auto left = std::vector<int>{};
	auto right = std::vector<int>{};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left));
	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
	QCOMPARE(overlapping.second, end(right));
}

void tst_Algorithm::findOverlappingRegionFirstEmptyTest()
{
	auto left = std::vector<int>{};
	auto right = std::vector<int>{1, 2, 3};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left));
	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
}

void tst_Algorithm::findOverlappingRegionSecondEmptyTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
	QCOMPARE(overlapping.second, end(right));
}

void tst_Algorithm::findOverlappingRegionSameTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{1, 2, 3};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left));
	QCOMPARE(overlapping.second, end(right));
}

void tst_Algorithm::findOverlappingRegionRemoveFirstTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{2, 3};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left) + 1);
	QCOMPARE(overlapping.second, end(right));
}

void tst_Algorithm::findOverlappingRegionAddOneTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{1, 2, 3, 4};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left));
	QCOMPARE(overlapping.second, end(right) - 1);
}

void tst_Algorithm::findOverlappingRegionOverlapingTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{2, 3, 4};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left) + 1);
	QCOMPARE(overlapping.second, end(right) - 1);
}

void tst_Algorithm::findOverlappingRegionDifferentTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{4, 5, 6};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
}

void tst_Algorithm::findOverlappingRegionOverlapingDifferentTest()
{
	auto left = std::vector<int>{1, 2, 3, 5, 6, 7};
	auto right = std::vector<int>{1, 2, 3, 4, 6, 7};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
}

QTEST_APPLESS_MAIN(tst_Algorithm)
#include "tst-algorithm.moc"
