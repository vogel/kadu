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

#include "formatted-string/text-converter-service.h"

#include <QtTest/QtTest>
#include <QtXml/QDomDocument>

class TextConverterServiceTest : public QObject
{
	Q_OBJECT

private slots:
	void shouldProperlyConvertHtmlToPlainText_data();
	void shouldProperlyConvertHtmlToPlainText();

	void shouldProperlyConvertPlainTextToHtml_data();
	void shouldProperlyConvertPlainTextToHtml();

};

void TextConverterServiceTest::shouldProperlyConvertHtmlToPlainText_data()
{
	QTest::addColumn<QString>("html");
	QTest::addColumn<QString>("plain");

	QTest::newRow("simple")
		<< R"(simple message)"
		<< R"(simple message)";
	QTest::newRow("formatted message")
		<< R"(fo<span style="font-weight:600;">rm</span><span style="font-weight:600;font-style:italic;">att</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">ed </span><span style="font-weight:600;font-style:italic;">m</span><span style="font-weight:600;">essag</span>e)"
		<< R"(formatted message)";
	QTest::newRow("complicated formatted message")
		<< R"(c<span style="font-weight:600;">omp</span>licated formatted me<span style="font-weight:600;">ssag</span>e)"
		<< R"(complicated formatted message)";
	QTest::newRow("multiline message")
		<< R"(multiline<br/>message)"
		<< "multiline\nmessage";
}

void TextConverterServiceTest::shouldProperlyConvertHtmlToPlainText()
{
	QFETCH(QString, html);
	QFETCH(QString, plain);

	TextConverterService textConverterService;
	auto result = textConverterService.htmlToPlain(html);

	QCOMPARE(result, plain);
}

void TextConverterServiceTest::shouldProperlyConvertPlainTextToHtml_data()
{
	QTest::addColumn<QString>("plain");
	QTest::addColumn<QString>("html");

	QTest::newRow("simple")
		<< "simple message"
		<< "simple message";
	QTest::newRow("with new line")
		<< "with\nnew line"
		<< "with<br />new line";
	QTest::newRow("with less and greater")
		<< "with < and >"
		<< "with &lt; and &gt;";
	QTest::newRow("with amp")
		<< "with &&"
		<< "with &amp;&amp;";
}

void TextConverterServiceTest::shouldProperlyConvertPlainTextToHtml()
{
	QFETCH(QString, plain);
	QFETCH(QString, html);

	TextConverterService textConverterService;
	auto result = textConverterService.plainToHtml(plain);

	QCOMPARE(result, html);
}

QTEST_APPLESS_MAIN(TextConverterServiceTest)
#include "text-converter-service.test.moc"
