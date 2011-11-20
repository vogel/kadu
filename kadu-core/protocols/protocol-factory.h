/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include <QtGui/QValidator>

#include "contacts/contact-details.h"
#include "exports.h"

class Account;
class AccountDetails;
class AccountAddWidget;
class AccountCreateWidget;
class AccountEditWidget;
class AccountShared;
class Buddy;
class ConfigurationWindow;
class Contact;
class ContactShared;
class KaduIcon;
class Protocol;
class ProtocolMenuManager;
class StoragePoint;

enum StatusType;

class KADUAPI ProtocolFactory : public QObject
{
	Q_OBJECT

public:
	virtual Protocol * createProtocolHandler(Account account) = 0;
	virtual AccountDetails * createAccountDetails(AccountShared *accountShared) = 0;
	virtual ContactDetails * createContactDetails(ContactShared *contactShared) = 0;
	virtual AccountAddWidget * newAddAccountWidget(bool showButtons, QWidget *parent = 0) = 0;
	virtual AccountCreateWidget * newCreateAccountWidget(bool showButtons, QWidget *parent = 0) = 0;
	virtual AccountEditWidget * newEditAccountWidget(Account, QWidget *parent = 0) = 0;
	virtual QWidget * newContactPersonalInfoWidget(Contact contact, QWidget *parent = 0) = 0;
	virtual ProtocolMenuManager * protocolMenuManager() { return 0; }
	virtual QList<StatusType> supportedStatusTypes() = 0;
	virtual QString idLabel() = 0;
	virtual QValidator::State validateId(QString id) = 0;
	virtual bool canRegister() { return true; }
	virtual bool canRemoveAvatar() { return true; } // this is so lame for gadu-gadu, so so lame ...

	virtual QString name() = 0;
	virtual QString displayName() = 0;

	virtual KaduIcon icon() = 0;

};

Q_DECLARE_METATYPE(ProtocolFactory *)

#endif // PROTOCOL_FACTORY_H
