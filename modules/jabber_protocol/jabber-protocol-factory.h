/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_PROTOCOL_FACTORY_H
#define JABBER_PROTOCOL_FACTORY_H

#include "protocols/protocol-factory.h"

class JabberProtocolFactory : public ProtocolFactory
{
	Q_DISABLE_COPY(JabberProtocolFactory)

	static JabberProtocolFactory *Instance;
	QList<StatusType *> SupportedStatusTypes;
	QRegExp IdRegularExpression;

	JabberProtocolFactory();

public:
	static JabberProtocolFactory * instance();

	virtual Account * newAccount();
	virtual Account * loadAccount(StoragePoint *accountStoragePoint);
	virtual ContactAccountData * newContactAccountData(Account *account, Contact contact, const QString &id);
	virtual ContactAccountData * loadContactAccountData(Account *account, Contact contact);
	virtual ContactAccountData * loadContactAccountData(StoragePoint *storagePoint);
	virtual AccountCreateWidget * newCreateAccountWidget(QWidget *parent);
	virtual AccountEditWidget* newEditAccountWidget(Account*, QWidget*);
	virtual ContactAccountDataWidget * newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent = 0);
	virtual QList<StatusType *> supportedStatusTypes();
	virtual QString idLabel();
	virtual QRegExp idRegularExpression();

	virtual QString name() { return "jabber"; }
	virtual QString displayName() { return "Jabber"; }
	virtual QString iconName();

};

#endif // JABBER_PROTOCOL_FACTORY_H
