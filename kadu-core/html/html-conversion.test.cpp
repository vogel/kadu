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

#include "html/html-conversion.h"
#include "html/html-string.h"
#include "html/normalized-html-string.h"

#include <QtTest/QtTest>
#include <QtXml/QDomDocument>

class HtmlConversionTest : public QObject
{
    Q_OBJECT

private slots:
    void shouldProperlyConvertHtmlToPlainText_data();
    void shouldProperlyConvertHtmlToPlainText();

    void shouldProperlyConvertPlainTextToHtml_data();
    void shouldProperlyConvertPlainTextToHtml();

    void shouldProperlyNormalizeHtml_data();
    void shouldProperlyNormalizeHtml();

    void shouldRemoveScriptTag_data();
    void shouldRemoveScriptTag();
};

void HtmlConversionTest::shouldProperlyConvertHtmlToPlainText_data()
{
    QTest::addColumn<QString>("html");
    QTest::addColumn<QString>("plain");

    QTest::newRow("simple") << R"(simple message)"
                            << R"(simple message)";
    QTest::newRow("formatted message")
        << R"(fo<span style="font-weight:600;">rm</span><span style="font-weight:600;font-style:italic;">att</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">ed </span><span style="font-weight:600;font-style:italic;">m</span><span style="font-weight:600;">essag</span>e)"
        << R"(formatted message)";
    QTest::newRow("complicated formatted message")
        << R"(c<span style="font-weight:600;">omp</span>licated formatted me<span style="font-weight:600;">ssag</span>e)"
        << R"(complicated formatted message)";
    QTest::newRow("multiline message") << R"(multiline<br/>message)"
                                       << "multiline\nmessage";
}

void HtmlConversionTest::shouldProperlyConvertHtmlToPlainText()
{
    QFETCH(QString, html);
    QFETCH(QString, plain);

    auto result = htmlToPlain(HtmlString{html});
    auto normalizedResult = htmlToPlain(normalizeHtml(HtmlString{html}));

    QCOMPARE(result, plain);
    QCOMPARE(normalizedResult, plain);
}

void HtmlConversionTest::shouldProperlyConvertPlainTextToHtml_data()
{
    QTest::addColumn<QString>("plain");
    QTest::addColumn<QString>("html");

    QTest::newRow("simple") << "simple message"
                            << "simple message";
    QTest::newRow("with new line") << "with\nnew line"
                                   << "with<br />new line";
    QTest::newRow("with less and greater") << "with < and >"
                                           << "with &lt; and &gt;";
    QTest::newRow("with amp") << "with &&"
                              << "with &amp;&amp;";
}

void HtmlConversionTest::shouldProperlyConvertPlainTextToHtml()
{
    QFETCH(QString, plain);
    QFETCH(QString, html);

    auto result = plainToHtml(plain).string();

    QCOMPARE(result, html);
}

void HtmlConversionTest::shouldProperlyNormalizeHtml_data()
{
    QTest::addColumn<QString>("html");
    QTest::addColumn<QString>("normalizedHtml");

    QTest::newRow("simple") << R"(simple message)"
                            << R"(simple message)";
    QTest::newRow("simple with span") << R"(<span>simple message</span>)"
                                      << R"(<span>simple message</span>)";
    QTest::newRow("with new line") << R"(with\nnew line)"
                                   << R"(with\nnew line)";
    QTest::newRow("with br") << R"(with br <br>)"
                             << R"(with br <br/>)";
    QTest::newRow("with brbr") << R"(with brbr <br><br>)"
                               << R"(with brbr <br/><br/>)";
    QTest::newRow("invalid html") << R"(invalid html <)"
                                  << R"(invalid html &lt;)";
    QTest::newRow("formatted")
        << R"(fo<span style="font-weight:600;">rm</span><span style="font-weight:600;font-style:italic;">atte</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">d<br/><br/>m</span><span style="font-weight:600;font-style:italic;">es</span><span style="font-weight:600;">s</span>age)"
        << R"(fo<span style="font-weight:600;">rm</span><span style="font-weight:600;font-style:italic;">atte</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">d<br/><br/>m</span><span style="font-weight:600;font-style:italic;">es</span><span style="font-weight:600;">s</span>age)";
    QTest::newRow("simple image") << R"(<img name="a8a4f03700000432">)"
                                  << R"(<img src="a8a4f03700000432"/>)";
    QTest::newRow("two images")
        << R"(message with <img name="77a1a0f10000066c"> two images <img name="6652381000000624"> and text)"
        << R"(message with <img src="77a1a0f10000066c"/> two images <img src="6652381000000624"/> and text)";
    QTest::newRow("formatted message with two images")
        << R"(fo<span style="font-weight:600;">rma</span><span style="font-weight:600;font-style:italic;">tted<br/></span><img name="77a1a0f10000066c"><span style="font-weight:600;font-style:italic;"><br/>mes</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">sage w</span><span style="font-weight:600;font-style:italic;">ith<br/></span><img name="9b5725c300000ec7"><span style="font-weight:600;font-style:italic;"><br/>tw</span><span style="font-weight:600;">o messag</span>es)"
        << R"(fo<span style="font-weight:600;">rma</span><span style="font-weight:600;font-style:italic;">tted<br/></span><img src="77a1a0f10000066c"/><span style="font-weight:600;font-style:italic;"><br/>mes</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">sage w</span><span style="font-weight:600;font-style:italic;">ith<br/></span><img src="9b5725c300000ec7"/><span style="font-weight:600;font-style:italic;"><br/>tw</span><span style="font-weight:600;">o messag</span>es)";
    QTest::newRow("multiline string") << R"(multiline<br>string)"
                                      << R"(multiline<br/>string)";
    QTest::newRow("multiline fromatted string") << R"(mul<span style="font-weight:600;">tiline<br>strin</span>g)"
                                                << R"(mul<span style="font-weight:600;">tiline<br/>strin</span>g)";
}

void HtmlConversionTest::shouldProperlyNormalizeHtml()
{
    QFETCH(QString, html);
    QFETCH(QString, normalizedHtml);

    auto result = normalizeHtml(HtmlString{html}).string();

    QCOMPARE(result, normalizedHtml);
}

void HtmlConversionTest::shouldRemoveScriptTag_data()
{
    QTest::addColumn<QString>("html");

    QTest::newRow("with script") << R"(here is script: <script>window.open("some javascript");</script>)";
    QTest::newRow("with typed script")
        << R"(here is script: <script type="javascript">window.open("some javascript");</script>)";
}

void HtmlConversionTest::shouldRemoveScriptTag()
{
    QFETCH(QString, html);

    auto result = normalizeHtml(HtmlString{html}).string();

    QVERIFY(!result.contains("<script"));
}

QTEST_APPLESS_MAIN(HtmlConversionTest)
#include "html-conversion.test.moc"
