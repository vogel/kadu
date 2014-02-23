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
	void sequenceDifferenceEmptyTest();
	void sequenceDifferenceFirstEmptyTest();
	void sequenceDifferenceSecondEmptyTest();
	void sequenceDifferenceSameTest();
	void sequenceDifferenceRemoveFirstTest();
	void sequenceDifferenceAddOneTest();
	void sequenceDifferenceOverlapingTest();
	void sequenceDifferenceDifferentTest();
	void sequenceDifferenceOverlapingDifferentTest();

};

void tst_Algorithm::sequenceDifferenceEmptyTest()
{
	auto left = std::vector<int>{};
	auto right = std::vector<int>{};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, begin(left));
	QCOMPARE(difference.first, end(left));
	QCOMPARE(difference.second, begin(right));
	QCOMPARE(difference.second, end(right));
}

void tst_Algorithm::sequenceDifferenceFirstEmptyTest()
{
	auto left = std::vector<int>{};
	auto right = std::vector<int>{1, 2, 3};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, begin(left));
	QCOMPARE(difference.first, end(left));
	QCOMPARE(difference.second, begin(right));
}

void tst_Algorithm::sequenceDifferenceSecondEmptyTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, end(left));
	QCOMPARE(difference.second, begin(right));
	QCOMPARE(difference.second, end(right));
}

void tst_Algorithm::sequenceDifferenceSameTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{1, 2, 3};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, begin(left));
	QCOMPARE(difference.second, end(right));
}

void tst_Algorithm::sequenceDifferenceRemoveFirstTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{2, 3};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, begin(left) + 1);
	QCOMPARE(difference.second, end(right));
}

void tst_Algorithm::sequenceDifferenceAddOneTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{1, 2, 3, 4};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, begin(left));
	QCOMPARE(difference.second, end(right) - 1);
}

void tst_Algorithm::sequenceDifferenceOverlapingTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{2, 3, 4};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, begin(left) + 1);
	QCOMPARE(difference.second, end(right) - 1);
}

void tst_Algorithm::sequenceDifferenceDifferentTest()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{4, 5, 6};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, end(left));
	QCOMPARE(difference.second, begin(right));
}

void tst_Algorithm::sequenceDifferenceOverlapingDifferentTest()
{
	auto left = std::vector<int>{1, 2, 3, 5, 6, 7};
	auto right = std::vector<int>{1, 2, 3, 4, 6, 7};
	auto difference = sequence_difference(begin(left), end(left), begin(right), end(right));

	QCOMPARE(difference.first, end(left));
	QCOMPARE(difference.second, begin(right));
}

QTEST_APPLESS_MAIN(tst_Algorithm)
#include "tst-algorithm.moc"
