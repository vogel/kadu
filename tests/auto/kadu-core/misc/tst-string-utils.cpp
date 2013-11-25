/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtTest/QtTest>

#include "misc/string-utils.h"

class tst_StringUtils : public QObject
{
	Q_OBJECT

private slots:
	void ellipsisTest_data();
	void ellipsisTest();

};

void tst_StringUtils::ellipsisTest_data()
{
	QTest::addColumn<QString>("input");
	QTest::addColumn<int>("maxLen");
	QTest::addColumn<QString>("expected");

	QTest::newRow("string,0") << "string" << 0 << "";
	QTest::newRow("string,0") << "string" << 1 << "s";
	QTest::newRow("string,5") << "string" << 2 << "st...";
	QTest::newRow("string,3") << "string" << 3 << "str...";
	QTest::newRow("string,6") << "string" << 6 << "string";
}

void tst_StringUtils::ellipsisTest()
{
	for (int i = 0; i < 5; i++)
	{
		QFETCH(QString, input);
		QFETCH(int, maxLen);
		QFETCH(QString, expected);

		QCOMPARE(StringUtils::ellipsis(input, maxLen), expected);
	}
}

QTEST_APPLESS_MAIN(tst_StringUtils)
#include "tst-string-utils.moc"
