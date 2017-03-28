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

class SortedUniqueVectorTest : public QObject
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

    using Vector = sorted_unique_vector<int, SortedUniqueVectorTest::compareInt, SortedUniqueVectorTest::equalInt>;

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

void SortedUniqueVectorTest::emptyTest()
{
    auto data = Vector{};

    QVERIFY(data.empty());
    QCOMPARE(data.size(), size_t{0});
    QCOMPARE(data.content(), (std::vector<int>{}));
}

void SortedUniqueVectorTest::clearTest()
{
    auto data = Vector{std::vector<int>{1, 4, 5, 2}};

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{4});

    data.clear();

    QVERIFY(data.empty());
    QCOMPARE(data.size(), size_t{0});
}

void SortedUniqueVectorTest::conversionFromUniqueUnsortedTest()
{
    auto data = Vector{std::vector<int>{1, 4, 5, 2}};

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{4});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void SortedUniqueVectorTest::conversionFromNonUniqueUnsortedTest()
{
    auto data = Vector{std::vector<int>{1, 4, 5, 2, 1, 4, 5, 2}};

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{4});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void SortedUniqueVectorTest::conversionFromUniqueSortedTest()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{4});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void SortedUniqueVectorTest::conversionFromNonUniqueSortedTest()
{
    auto data = Vector{std::vector<int>{1, 1, 2, 2, 4, 4, 5, 5}};

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{4});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void SortedUniqueVectorTest::addLessThanSmallest()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    data.add(0);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{5});
    QCOMPARE(data.content(), (std::vector<int>{0, 1, 2, 4, 5}));
}

void SortedUniqueVectorTest::addSmallest()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    data.add(1);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{4});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void SortedUniqueVectorTest::addMiddle()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    data.add(3);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{5});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 3, 4, 5}));
}

void SortedUniqueVectorTest::addLargest()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    data.add(5);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{4});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void SortedUniqueVectorTest::addGreaterThanLargest()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    data.add(6);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{5});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6}));
}

void SortedUniqueVectorTest::mergeCopy()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    data.merge(data);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{4});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5}));
}

void SortedUniqueVectorTest::mergeListOfLessUnique()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    auto dataToAdd = Vector{std::vector<int>{0, -1, -2}};
    data.merge(dataToAdd);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{7});
    QCOMPARE(data.content(), (std::vector<int>{-2, -1, 0, 1, 2, 4, 5}));
}

void SortedUniqueVectorTest::mergeListOfLessAndEqual()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    auto dataToAdd = Vector{std::vector<int>{1, 0, -1, -2}};
    data.merge(dataToAdd);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{7});
    QCOMPARE(data.content(), (std::vector<int>{-2, -1, 0, 1, 2, 4, 5}));
}

void SortedUniqueVectorTest::mergeListOfMisc()
{
    auto data = Vector{std::vector<int>{0, 5, 10, 1, 3, 2}};
    auto dataToAdd = Vector{std::vector<int>{-1, 17, 2, 3, 4, 5}};
    data.merge(dataToAdd);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{9});
    QCOMPARE(data.content(), (std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 10, 17}));
}

void SortedUniqueVectorTest::mergeListOfMiscUnique()
{
    auto data = Vector{std::vector<int>{0, 5, 10, 1, 3, 2}};
    auto dataToAdd = Vector{std::vector<int>{-1, 17, 4}};
    data.merge(dataToAdd);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{9});
    QCOMPARE(data.content(), (std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 10, 17}));
}

void SortedUniqueVectorTest::mergeListOfGraterAndEqual()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    auto dataToAdd = Vector{std::vector<int>{7, 8, 6}};
    data.merge(dataToAdd);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{7});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6, 7, 8}));
}

void SortedUniqueVectorTest::mergeListOfGraterUnique()
{
    auto data = Vector{std::vector<int>{1, 2, 4, 5}};
    auto dataToAdd = Vector{std::vector<int>{7, 5, 6}};
    data.merge(dataToAdd);

    QVERIFY(!data.empty());
    QCOMPARE(data.size(), size_t{6});
    QCOMPARE(data.content(), (std::vector<int>{1, 2, 4, 5, 6, 7}));
}

QTEST_APPLESS_MAIN(SortedUniqueVectorTest)
#include "sorted-unique-vector.test.moc"
