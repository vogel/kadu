/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtWebKit/QWebFrame>

#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "misc/syntax-list.h"
#include "parser/parser.h"
#include "url-handlers/url-handler-manager.h"

#include "debug.h"
#include "emoticons.h"
#include "html_document.h"

#include "buddy-info-panel.h"

BuddyInfoPanel::BuddyInfoPanel(QWidget *parent) : KaduWebView(parent), MyBuddy(Buddy::null)
{
	configurationUpdated();
}

BuddyInfoPanel::~BuddyInfoPanel()
{
}

void BuddyInfoPanel::configurationUpdated()
{
	update();
}

void BuddyInfoPanel::update()
{
	QFont font = config_file.readFontEntry("Look", "PanelFont");
	QString fontFamily = font.family();
	QString fontSize;
	if (font.pointSize() > 0)
		fontSize = QString::number(font.pointSize()) + "pt";
	else
		fontSize = QString::number(font.pixelSize()) + "px";

	QString fontStyle = font.italic() ? "italic" : "normal";
	QString fontWeight = font.bold() ? "bold" : "normal";
	QString textDecoration = font.underline() ? "underline" : "none";
	QString backgroundColor = config_file.readColorEntry("Look", "InfoPanelBgColor").name();
	QString fontColor = config_file.readColorEntry("Look", "InfoPanelFgColor").name();

	Template = QString(
		"<html>"
		"	<head>"
		"		<style type='text/css'>"
		"html {"
		"	color: %1;"
		"	font: %2 %3 %4 %5;"
		"	text-decoration: %6;"
		"	margin: 0;"
		"	padding: 0;"
		"	background-color: %7;"
		"}"
		"div {"
		"	color: %1;"
		"	font: %2 %3 %4 %5;"
		"	text-decoration: %6;"
		"	margin: 0;"
		"	padding: 0;"
		"	background-color: %7;"
		"}"
		"		</style>"
		"	</head>"
		"	<body>"
		"		%8"
		"	</body>"
		"</html>"
		).arg(fontColor, fontStyle, fontWeight, fontSize, fontFamily, textDecoration, backgroundColor, "%1");

	Syntax = SyntaxList::readSyntax("infopanel", config_file.readEntry("Look", "InfoPanelSyntaxFile"),
		"<table><tr><td><img width=\"32\" height=\"32\" align=\"left\" valign=\"top\" src=\"file:///@{ManageUsersWindowIcon}\"></td><td> "
		"<div align=\"left\"> [<b>%a</b>][ (%u)] [<br>tel.: %m][<br>IP: %i]</div></td></tr></table> <hr> <b>%s</b> [<br>%d]");
	setHtml(QString("<body bgcolor=\"") + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	displayBuddy(MyBuddy);

	if (config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
		page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
	else
		page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}

void BuddyInfoPanel::displayBuddy(Buddy buddy)
{
	MyBuddy = buddy;

	if (Buddy::null == MyBuddy || !isVisible())
		return;

	HtmlDocument doc;
	doc.parseHtml(Parser::parse(Syntax, MyBuddy.preferredContact()));
	UrlHandlerManager::instance()->convertAllUrls(doc);

	if (EmoticonsStyleNone != (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") &&
			config_file.readBoolEntry("General", "ShowEmotPanel"))
		EmoticonsManager::instance()->expandEmoticons(doc,
				(EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));

	setHtml(Template.arg(doc.generateHtml()));

	kdebugf2();
}

void BuddyInfoPanel::styleFixup(QString &syntax)
{
	syntax = Template.arg(syntax);
}
