/*
 * %kadu copyright begin%
 * Copyright 2009 Juzef (juzefwt@tlen.pl)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef PROTOCOL_FACTORY_H
#define PROTOCOL_FACTORY_H

#include <QtCore/QString>
#include <QtGui/QDialog>

#include "contacts/contact-details.h"

class Account;
class AccountCreateWidget;
class AccountDetails;
class AccountAddWidget;
class AccountEditWidget;
class AccountShared;
class Buddy;
class ConfigurationWindow;
class Contact;
class ContactShared;
class ContactWidget;
class Protocol;
class ProtocolMenuManager;
class StatusType;
class StoragePoint;

class ProtocolFactory : public QObject
{
public:
	virtual Protocol * createProtocolHandler(Account account) = 0;
	virtual AccountDetails * createAccountDetails(AccountShared *accountShared) = 0;
	virtual ContactDetails * createContactDetails(ContactShared *contactShared) = 0;
	virtual AccountAddWidget * newAddAccountWidget(QWidget *parent = 0) = 0;
	virtual AccountCreateWidget * newCreateAccountWidget(QWidget *parent = 0) = 0;
	virtual AccountEditWidget * newEditAccountWidget(Account, QWidget *parent = 0) = 0;
	virtual ContactWidget * newContactWidget(Contact contact, QWidget *parent = 0) = 0;
	virtual ProtocolMenuManager * protocolMenuManager() { return 0; }
	virtual QList<StatusType *> supportedStatusTypes() = 0;
	virtual QString idLabel() = 0;
	virtual QRegExp idRegularExpression() = 0;

	virtual QString name() = 0;
	virtual QString displayName() = 0;

	virtual QString iconName()
	{
		return QString::null;
	}

};

#endif // PROTOCOL_FACTORY_H
