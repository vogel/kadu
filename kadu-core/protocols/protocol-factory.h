/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROTOCOL_FACTORY_H
#define PROTOCOL_FACTORY_H

#include <QtCore/QString>
#include <QtGui/QDialog>

class Account;
class AccountCreateWidget;
class AccountDetails;
class AccountEditWidget;
class ConfigurationWindow;
class Contact;
class ContactAccountData;
class ContactAccountDataWidget;
class Protocol;
class ProtocolMenuManager;
class StatusType;
class StoragePoint;

class ProtocolFactory : public QObject
{
public:
	virtual Protocol * createProtocolHandler(Account *account) = 0;
	virtual AccountDetails * createAccountDetails(Account *account) = 0;
	virtual ContactAccountData * newContactAccountData(Account *account, Contact contact, const QString &id) = 0;
	virtual ContactAccountData * loadContactAccountData(StoragePoint *storagePoint) = 0;
	virtual AccountCreateWidget * newCreateAccountWidget(QWidget *parent = 0) = 0;
	virtual AccountEditWidget * newEditAccountWidget(Account *, QWidget *parent = 0) = 0;
	virtual ContactAccountDataWidget * newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent = 0) = 0;
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
