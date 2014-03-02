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

#include "misc/sorted_vector.h"

#include <QtTest/QtTest>

class tst_SortedVector : public QObject
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

	using Vector = sorted_vector<int, tst_SortedVector::compareInt, tst_SortedVector::equalInt>;

private slots:
	void emptyTest();
	void clearTest();
	void conversionFromUnsortedTest();
	void conversionFromSortedTest();
	void addLessThanSmallest();
	void addSmallest();
	void addMiddle();
	void addLargest();
	void addGreaterThanLargest();
	void addCopy();
	void addListOfLessUnique();
	void addListOfLessAndEqual();
	void addListOfMisc();
	void addListOfMiscUnique();
	void addListOfGraterAndEqual();
	void addListOfGraterUnique();

};

void tst_SortedVector::emptyTest()
{
	auto data = Vector{};

	QVERIFY(data.empty());
	QCOMPARE(data.size(), 0ul);
	QCOMPARE(data.content(), (std::vector<int>{}));
}

void tst_SortedVector::clearTest()
{
	auto data = Vector{std::vector<int>{1, 4, 5, 2}};

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);

	data.clear();

	QVERIFY(data.empty());
	QCOMPARE(data.size(), 0ul);
}

void tst_SortedVector::conversionFromUnsortedTest()
{
	auto data = Vector{std::vector<int>{1, 4, 5, 2}};

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedVector::conversionFromSortedTest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedVector::addLessThanSmallest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(0);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 5ul);
	QCOMPARE(data.content(), (std::vector<int>{0, 1, 2, 4, 5}));
}

void tst_SortedVector::addSmallest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(1);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedVector::addMiddle()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(3);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 5ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 3, 4, 5}));
}

void tst_SortedVector::addLargest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(5);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedVector::addGreaterThanLargest()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(6);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 5ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6}));
}

void tst_SortedVector::addCopy()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	data.add(data);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 4ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void tst_SortedVector::addListOfLessUnique()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	auto dataToAdd = Vector{std::vector<int>{0, -1, -2}};
	data.add(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 7ul);
	QCOMPARE(data.content(), (std::vector<int>{-2, -1, 0, 1, 2, 4, 5}));
}

void tst_SortedVector::addListOfLessAndEqual()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	auto dataToAdd = Vector{std::vector<int>{1, 0, -1, -2}};
	data.add(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 7ul);
	QCOMPARE(data.content(), (std::vector<int>{-2, -1, 0, 1, 2, 4, 5}));
}

void tst_SortedVector::addListOfMisc()
{
	auto data = Vector{std::vector<int>{0, 5, 10, 1, 3, 2}};
	auto dataToAdd = Vector{std::vector<int>{-1, 17, 2, 3, 4, 5}};
	data.add(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 9ul);
	QCOMPARE(data.content(), (std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 10, 17}));
}

void tst_SortedVector::addListOfMiscUnique()
{
	auto data = Vector{std::vector<int>{0, 5, 10, 1, 3, 2}};
	auto dataToAdd = Vector{std::vector<int>{-1, 17, 4}};
	data.add(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 9ul);
	QCOMPARE(data.content(), (std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 10, 17}));
}

void tst_SortedVector::addListOfGraterAndEqual()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	auto dataToAdd = Vector{std::vector<int>{7, 8, 6}};
	data.add(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 7ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6, 7, 8}));
}

void tst_SortedVector::addListOfGraterUnique()
{
	auto data = Vector{std::vector<int>{1, 2, 4, 5}};
	auto dataToAdd = Vector{std::vector<int>{7, 5, 6}};
	data.add(dataToAdd);

	QVERIFY(!data.empty());
	QCOMPARE(data.size(), 6ul);
	QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6, 7}));
}

QTEST_APPLESS_MAIN(tst_SortedVector)
#include "tst-sorted-vector.moc"
