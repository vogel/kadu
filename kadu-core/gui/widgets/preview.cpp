/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtNetwork/QHostAddress>

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-account-data.h"
#include "parser/parser.h"
#include "status/status.h"
#include "status/status-type-manager.h"
#include "misc/misc.h"

#include "preview.h"

Preview::Preview(QWidget *parent)
	: KaduTextBrowser(parent)
{
	setFixedHeight(170);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	account = AccountManager::instance()->defaultAccount();
	Status status("Away", qApp->translate("@default", "Description"));

	//contact.addAccountData(ContactAccountData(account, "999999"));
	//ContactAccountData *contact_data = contact.accountData(account);
	//contact_data->setStatus(status);
	//contact_data->setAddressAndPort(QHostAddress(2130706433), 80);
	//contact_data->setDNSName("Gadu", "host.server.net");

	contact.setFirstName(qApp->translate("@default", "Mark"));
	contact.setLastName(qApp->translate("@default", "Smith"));
	contact.setNickName(qApp->translate("@default", "Jimbo"));
	contact.setDisplay(qApp->translate("@default", "Jimbo"));
	contact.setMobile("+48123456789");
	contact.setEmail("jimbo@mail.server.net");
	contact.setHomePhone("+481234567890");

}

Preview::~Preview()
{
	qDeleteAll(objectsToParse);
}

void Preview::syntaxChanged(const QString &content)
{
// 	viewport()->setUpdatesEnabled(false);

	QString syntax = content;
	emit needSyntaxFixup(syntax);

	QString text;

// 	setHtml("<body bgcolor=\"" + resetBackgroundColor + "\"></body>");
	int count = objectsToParse.count();

	if (count)
		for (int i = 0; i < count; i++)
			text += Parser::parse(syntax, account, contacts[i], objectsToParse.at(i));
	else
		text = Parser::parse(syntax, account, contact);

	emit needFixup(text);

	setHtml(text);

// 	viewport()->setUpdatesEnabled(true);
// 	viewport()->repaint();
}
