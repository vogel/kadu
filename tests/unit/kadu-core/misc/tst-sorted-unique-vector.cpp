/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/sorted-unique-vector.h"

#include <QtTest/QtTest>

class tst_SortedUniqueVector : public QObject
{
	Q_OBJECT

	static bool compareInt(const int &left, const int &right)
	{
		return left < right;
	}

	static bool equalInt(const int &left, const int &right)
	{
		return left == right;
	}

	using Vector = sorted_unique_vector<int, tst_SortedUniqueVector::compareInt, tst_SortedUniqueVector::equalInt>;

private slots:
	void emptyTest();
	void clearTest();
	void conversionFromUniqueUnsortedTest();
	void conversionFromNonUniqueUnsortedTest();
	void conversionFromUniqueSortedTest();
	void conversionFromNonUniqueSortedTest();
	void addLessThanSmallest();
	void addSmallest();
	void addMiddle();
	void addLargest();
	void addGreaterThanLargest();
	void mergeCopy();
	void mergeListOfLessUnique();
	void mergeListOfLessAndEqual();
	void mergeListOfMisc();
	void mergeListOfMiscUnique();
	void mergeListOfGraterAndEqual();
	void mergeListOfGraterUnique();

};

void tst_SortedUniqueVector::emptyTest()
{
	auto data = Vector{};

	QVERIFY(data.empty());
	QCOMPARE(data.size(), 0ul);
	QCOMPARE(data.content(), (std::vector<int>{}));
}

void tst_SortedUniqueVector::clearTest()
{
	auto data = Vector{std::vector<int>{1, 4, 5, 2}};

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);

	data.clear();

	QVERIFY(data.empty());
	QCOMPARE(data.size(), 0ul);
}

void tst_SortedUniqueVector::conversionFromUniqueUnsortedTest()
{
	auto data = Vector{std::vector<int>{1, 4, 5, 2}};

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedUniqueVector::conversionFromNonUniqueUnsortedTest()
{
	auto data = Vector{std::vector<int>{1, 4, 5, 2, 1, 4, 5, 2}};

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedUniqueVector::conversionFromUniqueSortedTest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedUniqueVector::conversionFromNonUniqueSortedTest()
{
	auto data = Vector{std::vector<int>{1, 1, 2, 2, 4, 4, 5, 5}};

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedUniqueVector::addLessThanSmallest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(0);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 5ul);
	QCOMPARE(data.content(), (std::vector<int>{0, 1, 2, 4, 5}));
}

void tst_SortedUniqueVector::addSmallest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(1);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedUniqueVector::addMiddle()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(3);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 5ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 3, 4, 5}));
}

void tst_SortedUniqueVector::addLargest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(5);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedUniqueVector::addGreaterThanLargest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(6);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 5ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6}));
}

void tst_SortedUniqueVector::mergeCopy()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.merge(data);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedUniqueVector::mergeListOfLessUnique()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	auto dataToAdd = Vector{std::vector<int>{0, -1, -2}};
	data.merge(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 7ul);
	QCOMPARE(data.content(), (std::vector<int>{-2, -1, 0, 1, 2, 4, 5}));
}

void tst_SortedUniqueVector::mergeListOfLessAndEqual()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	auto dataToAdd = Vector{std::vector<int>{1, 0, -1, -2}};
	data.merge(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 7ul);
	QCOMPARE(data.content(), (std::vector<int>{-2, -1, 0, 1, 2, 4, 5}));
}

void tst_SortedUniqueVector::mergeListOfMisc()
{
	auto data = Vector{std::vector<int>{0, 5, 10, 1, 3, 2}};
	auto dataToAdd = Vector{std::vector<int>{-1, 17, 2, 3, 4, 5}};
	data.merge(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 9ul);
	QCOMPARE(data.content(), (std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 10, 17}));
}

void tst_SortedUniqueVector::mergeListOfMiscUnique()
{
	auto data = Vector{std::vector<int>{0, 5, 10, 1, 3, 2}};
	auto dataToAdd = Vector{std::vector<int>{-1, 17, 4}};
	data.merge(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 9ul);
	QCOMPARE(data.content(), (std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 10, 17}));
}

void tst_SortedUniqueVector::mergeListOfGraterAndEqual()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	auto dataToAdd = Vector{std::vector<int>{7, 8, 6}};
	data.merge(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 7ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6, 7, 8}));
}

void tst_SortedUniqueVector::mergeListOfGraterUnique()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	auto dataToAdd = Vector{std::vector<int>{7, 5, 6}};
	data.merge(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 6ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6, 7}));
}

QTEST_APPLESS_MAIN(tst_SortedUniqueVector)
#include "tst-sorted-unique-vector.moc"
