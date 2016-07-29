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

#include "services/gadu-received-html-fixup-service.h"

#include <QtTest/QtTest>

class GaduReceivedHtmlFixupServiceTest : public QObject
{
	Q_OBJECT

private slots:
	void shouldKeepEmptyAsEmpty();
	void shouldKeepSimpleMessage();
	void shouldKeepFormattedMessage();
	void shouldFixSimpleImage();
	void shouldFixTwoImages();
	void shouldFixFormattedMessageWithTwoImages();

};

void GaduReceivedHtmlFixupServiceTest::shouldKeepEmptyAsEmpty()
{
	GaduReceivedHtmlFixupService service{};

	QCOMPARE(service.htmlFixup({}), QString{});
}

void GaduReceivedHtmlFixupServiceTest::shouldKeepSimpleMessage()
{
	GaduReceivedHtmlFixupService service{};

	auto content = QString{R"(<span>simple message</span>)"};
	QCOMPARE(service.htmlFixup(content), content);

}

void GaduReceivedHtmlFixupServiceTest::shouldKeepFormattedMessage()
{
	GaduReceivedHtmlFixupService service{};

	auto content = QString{R"(fo<span style="font-weight:600;">rm</span><span style="font-weight:600;font-style:italic;">atte</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">d<br/><br/>m</span><span style="font-weight:600;font-style:italic;">es</span><span style="font-weight:600;">s</span>age)"};
	QCOMPARE(service.htmlFixup(content), content);
}

void GaduReceivedHtmlFixupServiceTest::shouldFixSimpleImage()
{
	GaduReceivedHtmlFixupService service{};

	auto contentToFixUp = QString{R"(<img name="a8a4f03700000432">)"};
	auto contentFixedUp = QString{R"(<img src="a8a4f03700000432" />)"};
	QCOMPARE(service.htmlFixup(contentToFixUp), contentFixedUp);
}

void GaduReceivedHtmlFixupServiceTest::shouldFixTwoImages()
{
	GaduReceivedHtmlFixupService service{};

	auto contentToFixUp = QString{R"(message with <img name="77a1a0f10000066c"> two images <img name="6652381000000624"> and text)"};
	auto contentFixedUp = QString{R"(message with <img src="77a1a0f10000066c" /> two images <img src="6652381000000624" /> and text)"};
	QCOMPARE(service.htmlFixup(contentToFixUp), contentFixedUp);
}

void GaduReceivedHtmlFixupServiceTest::shouldFixFormattedMessageWithTwoImages()
{
	GaduReceivedHtmlFixupService service{};

	auto contentToFixUp = QString{R"(fo<span style="font-weight:600;">rma</span><span style="font-weight:600;font-style:italic;">tted<br/></span><img name="77a1a0f10000066c"><span style="font-weight:600;font-style:italic;"><br/>mes</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">sage w</span><span style="font-weight:600;font-style:italic;">ith<br/></span><img name="9b5725c300000ec7"><span style="font-weight:600;font-style:italic;"><br/>tw</span><span style="font-weight:600;">o messag</span>es)"};
	auto contentFixedUp = QString{R"(fo<span style="font-weight:600;">rma</span><span style="font-weight:600;font-style:italic;">tted<br/></span><img src="77a1a0f10000066c" /><span style="font-weight:600;font-style:italic;"><br/>mes</span><span style="font-weight:600;font-style:italic;text-decoration:underline;">sage w</span><span style="font-weight:600;font-style:italic;">ith<br/></span><img src="9b5725c300000ec7" /><span style="font-weight:600;font-style:italic;"><br/>tw</span><span style="font-weight:600;">o messag</span>es)"};
	QCOMPARE(service.htmlFixup(contentToFixUp), contentFixedUp);
}

QTEST_APPLESS_MAIN(GaduReceivedHtmlFixupServiceTest)
#include "gadu-received-html-fixup-service.test.moc"
