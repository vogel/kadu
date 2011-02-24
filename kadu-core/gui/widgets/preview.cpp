/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtNetwork/QHostAddress>

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "parser/parser.h"
#include "status/status.h"
#include "status/status-type-manager.h"

#include "misc/misc.h"

#include "preview.h"

Preview::Preview(QWidget *parent)
	: KaduWebView(parent), contact(Contact::null)
{
	setFixedHeight(170);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

Preview::~Preview()
{
	qDeleteAll(objectsToParse);
}

void Preview::syntaxChanged(const QString &content)
{
	QString syntax = content;
	emit needSyntaxFixup(syntax);

	QString text;

// 	setHtml("<body bgcolor=\"" + resetBackgroundColor + "\"></body>");
	int count = objectsToParse.count();

	if (count)
	{
		Contact contact = *contacts.constBegin();
		for (int i = 0; i < count; i++)
			text += Parser::parse(syntax, BuddyOrContact(contact), objectsToParse.at(i));
	}
	else
		text = Parser::parse(syntax, BuddyOrContact(Buddy::dummy()));

	emit needFixup(text);

	setHtml(text);
}
