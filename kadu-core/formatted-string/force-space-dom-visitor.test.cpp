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

#include "formatted-string/force-space-dom-visitor.h"

#include "dom/dom-processor.h"

#include <QtTest/QtTest>
#include <QtXml/QDomDocument>

class ForceSpaceDomVisitorTest : public QObject
{
	Q_OBJECT

private slots:
	void shouldProperlyForceSpace_data();
	void shouldProperlyForceSpace();

};

void ForceSpaceDomVisitorTest::shouldProperlyForceSpace_data()
{
	QTest::addColumn<QString>("before");
	QTest::addColumn<QString>("after");

	QTest::newRow("simple")
		<< R"(test message)"
		<< R"(test&nbsp;message)";
	QTest::newRow("simple html")
		<< R"(<span>test message</span>)"
		<< R"(<span>test&nbsp;message</span>)";
	QTest::newRow("simple html with two tags")
		<< R"(<span>test message</span><span>test message 2</span>)"
		<< R"(<span>test&nbsp;message</span><span>test&nbsp;message&nbsp;2</span>)";
	QTest::newRow("multiple spaces")
		<< R"(  test   message  )"
		<< R"(&nbsp;&nbsp;test&nbsp;&nbsp;&nbsp;message&nbsp;&nbsp;)";
	QTest::newRow("multiple spaces in simple html")
		<< R"(<span>  test   message  </span>)"
		<< R"(<span>&nbsp;&nbsp;test&nbsp;&nbsp;&nbsp;message&nbsp;&nbsp;</span>)";
	QTest::newRow("multiple spaces in simple html with two tags")
		<< R"(<span>  test   message  </span><span>  test   message  2</span>)"
		<< R"(<span>&nbsp;&nbsp;test&nbsp;&nbsp;&nbsp;message&nbsp;&nbsp;</span><span>&nbsp;&nbsp;test&nbsp;&nbsp;&nbsp;message&nbsp;&nbsp;2</span>)";
	QTest::newRow("multiple spaces inside link")
		<< R"(<a href="http://www.example.com">  link  with     multiple spaces</a>)"
		<< R"(<a href="http://www.example.com">&nbsp;&nbsp;link&nbsp;&nbsp;with&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;multiple&nbsp;spaces</a>)";
	QTest::newRow("replace tab")
		<< "\there is tab"
		<< "&emsp;here&nbsp;is&nbsp;tab";
	QTest::newRow("remove multiple spaces inside tag")
		<< R"(<a  href="http://www.example.com">link</a>)"
		<< R"(<a href="http://www.example.com">link</a>)";
}

void ForceSpaceDomVisitorTest::shouldProperlyForceSpace()
{
	QFETCH(QString, before);
	QFETCH(QString, after);

	QDomDocument domDocument;
	// force content to be valid HTML with only one root
	domDocument.setContent(QString("<div>%1</div>").arg(before));

	auto forceNbspDomVisitor = ForceSpaceDomVisitor{};
	auto domProcessor = DomProcessor{domDocument};
	domProcessor.accept(&forceNbspDomVisitor);

	auto result = domDocument.toString(-1).trimmed();
	// remove <div></div>
	QVERIFY(result.startsWith(QStringLiteral("<div>")));
	QVERIFY(result.endsWith(QStringLiteral("</div>")));
	auto recreted = result.mid(static_cast<int>(qstrlen("<div>")), result.length() - static_cast<int>(qstrlen("<div></div>")));

	QCOMPARE(recreted, after);
}

QTEST_APPLESS_MAIN(ForceSpaceDomVisitorTest)
#include "force-space-dom-visitor.test.moc"
