/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-html-visitor.h"

#include <QtTest/QtTest>

class FormattedStringFactoryTest : public QObject
{
	Q_OBJECT

private slots:
	void shouldProperlyParseHtml_data();
	void shouldProperlyParseHtml();

};

void FormattedStringFactoryTest::shouldProperlyParseHtml_data()
{
	QTest::addColumn<QString>("html");
	QTest::addColumn<QString>("result");

	QTest::newRow("simple")
		<< "test message"
		<< "test message";
	QTest::newRow("simple html")
		<< "<span>test message</span>"
		<< "test message";
	QTest::newRow("formatted")
		<< "<span style=\"\">test </span><span style=\"font-weight:600;\">f</span><span style=\"font-weight:600;font-style:italic;\">o</span><span style=\"font-weight:600;font-style:italic;text-decoration:underline;\">rmatt</span><span style=\"font-weight:600;font-style:italic;\">e</span><span style=\"font-weight:600;\">d</span><span style=\"\"> message</span>"
		<< "test <span style=\"font-weight:600;\">f</span><span style=\"font-weight:600;font-style:italic;\">o</span><span style=\"font-weight:600;font-style:italic;text-decoration:underline;\">rmatt</span><span style=\"font-weight:600;font-style:italic;\">e</span><span style=\"font-weight:600;\">d</span> message";
	QTest::newRow("with image absolute path")
		<< "test message with image <img src=\"/image.png\">"
		<< "test message with image <img class=\"scalable\" src=\"file:///image.png\" name=\"/image.png\" />";
	QTest::newRow("with image id")
		<< "test message with image <img src=\"1075d4da00000146\">"
		<< "test message with image <img class=\"scalable\" src=\"kaduimg:///1075d4da00000146\" name=\"1075d4da00000146\" />";
	QTest::newRow("with image kaduimg")
		<< "test message with image <img src=\"kaduimg:///1075d4da00000146\">"
		<< "test message with image <img class=\"scalable\" src=\"kaduimg:///1075d4da00000146\" name=\"kaduimg:///1075d4da00000146\" />";
	QTest::newRow("with image absolute path xml")
		<< "test message with image <img src=\"/image.png\" />"
		<< "test message with image <img class=\"scalable\" src=\"file:///image.png\" name=\"/image.png\" />";
	QTest::newRow("with image id xml")
		<< "test message with image <img src=\"1075d4da00000146\" />"
		<< "test message with image <img class=\"scalable\" src=\"kaduimg:///1075d4da00000146\" name=\"1075d4da00000146\" />";
	QTest::newRow("with image kaduimg xml")
		<< "test message with image <img src=\"kaduimg:///1075d4da00000146\" />"
		<< "test message with image <img class=\"scalable\" src=\"kaduimg:///1075d4da00000146\" name=\"kaduimg:///1075d4da00000146\" />";
	QTest::newRow("with indentation simple")
		<< "  test message<br>    with    indentation"
		<< "  test message<br/>    with    indentation";
	QTest::newRow("with indentation")
		<< "<span>  test message<br>    with    indentation</span>"
		<< "  test message<br/>    with    indentation";
}

void FormattedStringFactoryTest::shouldProperlyParseHtml()
{
	QFETCH(QString, html);
	QFETCH(QString, result);

	FormattedStringFactory formattedStringFactory{};
	auto formattedString = formattedStringFactory.fromHtml(html);

	FormattedStringHtmlVisitor htmlVisitor{};
	formattedString->accept(&htmlVisitor);
	auto recreted = htmlVisitor.result();

	QCOMPARE(recreted, result);
}

QTEST_APPLESS_MAIN(FormattedStringFactoryTest)
#include "formatted-string-factory.test.moc"
