/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "avatars/avatar.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "emoticons/emoticons-manager.h"
#include "misc/syntax-list.h"
#include "parser/parser.h"
#include "url-handlers/url-handler-manager.h"

#include "debug.h"
#include "html_document.h"

#include "buddy-info-panel.h"

BuddyInfoPanel::BuddyInfoPanel(QWidget *parent) : KaduWebView(parent)
{
	configurationUpdated();

	QPalette p = palette();
	p.setBrush(QPalette::Base, Qt::transparent);
	page()->setPalette(p);
	setAttribute(Qt::WA_OpaquePaintEvent, false);

	connect(BuddyPreferredManager::instance(), SIGNAL(buddyUpdated(Buddy&)), this, SLOT(buddyUpdated(Buddy&)));
}

BuddyInfoPanel::~BuddyInfoPanel()
{
	disconnect(BuddyPreferredManager::instance(), SIGNAL(buddyUpdated(Buddy&)), this, SLOT(buddyUpdated(Buddy&)));
}

void BuddyInfoPanel::configurationUpdated()
{
	setUserFont(config_file.readFontEntry("Look", "PanelFont").toString(), true);

	EmoticonsManager::instance()->configurationUpdated();

	update();
}

void BuddyInfoPanel::buddyUpdated(Buddy &buddy)
{
	if (buddy == Item.toBuddy())
		update();
}

void BuddyInfoPanel::update()
{
	if (Core::instance()->isClosing())
		return;

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
	QString fontColor = config_file.readColorEntry("Look", "InfoPanelFgColor").name();
	bool backgroundFilled = config_file.readBoolEntry("Look", "InfoPanelBgFilled");
	if (backgroundFilled)
		BackgroundColor = config_file.readColorEntry("Look", "InfoPanelBgColor").name();
	else
		BackgroundColor = "transparent";

	Template = QString(
		"<html>"
		"	<head>"
		"		<style type='text/css'>"
		"		html {"
		"			color: %1;"
		"			font: %2 %3 %4 %5;"
		"			text-decoration: %6;"
		"			margin: 0;"
		"			padding: 0;"
		"			background-color: %7;"
		"		}"
		"		div {"
		"			color: %1;"
		"			font: %2 %3 %4 %5;"
		"			text-decoration: %6;"
		"			margin: 0;"
		"			padding: 0;"
		"			background-color: %7;"
		"		}"
		"		table {"
		"			color: %1;"
		"			font: %2 %3 %4 %5;"
		"			text-decoration: %6;"
		"		}"
		"		</style>"
		"	</head>"
		"	<body>"
		"		%8"
		"	</body>"
		"</html>"
		).arg(fontColor, fontStyle, fontWeight, fontSize, fontFamily, textDecoration, BackgroundColor, "%1");

	QString syntaxFile = config_file.readEntry("Look", "InfoPanelSyntaxFile", "ultr");
	if (syntaxFile == "default")
	{
		syntaxFile = "Old Default";
		config_file.writeEntry("Look", "InfoPanelSyntaxFile", syntaxFile);
	}

	Syntax = SyntaxList::readSyntax("infopanel", syntaxFile,
		"<table><tr><td><img width=\"32\" height=\"32\" align=\"left\" valign=\"top\" src=\"file:///@{x-office-address-book:32x32}\"></td><td> "
		"<div align=\"left\"> [<b>%a</b>][ (%u)] [<br>tel.: %m][<br>IP: %i]</div></td></tr></table> <hr> <b>%s</b> [<br>%d]");
	displayItem(Item);

	if (config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
		page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
	else
		page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}

void BuddyInfoPanel::connectItem()
{
	Buddy buddy = Item.toBuddy();
	if (buddy)
	{
		connect(buddy, SIGNAL(updated()), this, SLOT(update()));
		if (buddy.buddyAvatar())
			connect(buddy.buddyAvatar(), SIGNAL(updated()), this, SLOT(update()));
	}

	Contact contact = Item.toContact();
	if (contact)
	{
		connect(contact, SIGNAL(updated()), this, SLOT(update()));
		if (contact.contactAvatar())
			connect(contact.contactAvatar(), SIGNAL(updated()), this, SLOT(update()));
	}
}

void BuddyInfoPanel::disconnectItem()
{
	Buddy buddy = Item.toBuddy();
	if (buddy)
	{
		disconnect(buddy, SIGNAL(updated()), this, SLOT(update()));
		if (buddy.buddyAvatar())
			disconnect(buddy.buddyAvatar(), SIGNAL(updated()), this, SLOT(update()));
	}

	Contact contact = Item.toContact();
	if (contact)
	{
		disconnect(contact, SIGNAL(updated()), this, SLOT(update()));
		if (contact.contactAvatar())
			disconnect(contact.contactAvatar(), SIGNAL(updated()), this, SLOT(update()));
	}
}

void BuddyInfoPanel::displayItem(Talkable item)
{
	disconnectItem();
	Item = item;
	connectItem();

	if (!isVisible())
		return;

	if (item.isEmpty())
	{
		setHtml("<body bgcolor=\"" + BackgroundColor + "\"></body>");
		return;
	}

	HtmlDocument doc;
	doc.parseHtml(Parser::parse(Syntax, item));
	UrlHandlerManager::instance()->convertAllUrls(doc, false);

	if (EmoticonsStyleNone != (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") &&
			config_file.readBoolEntry("General", "ShowEmotPanel"))
		EmoticonsManager::instance()->expandEmoticons(doc,
				(EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));

	setHtml(Template.arg(doc.generateHtml()));
}

void BuddyInfoPanel::setVisible(bool visible)
{
	QWidget::setVisible(visible);

	if (visible)
		displayItem(Item);
}

void BuddyInfoPanel::styleFixup(QString &syntax)
{
	syntax = Template.arg(syntax);
}
