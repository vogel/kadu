/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_PROTOCOL_FACTORY_H
#define TLEN_PROTOCOL_FACTORY_H

#include "protocols/protocol-factory.h"
#include "misc/misc.h"

class TlenProtocolFactory : public ProtocolFactory
{
	Q_DISABLE_COPY(TlenProtocolFactory)

	static TlenProtocolFactory *Instance;
	QList<StatusType *> SupportedStatusTypes;
	QRegExp IdRegularExpression;

	TlenProtocolFactory();

public:
	static TlenProtocolFactory * instance();

	virtual Account * newAccount();
	virtual Account * loadAccount(StoragePoint *accountStoragePoint);
	virtual Contact * newContact(Account *account, Contact contact, const QString &id);
	virtual Contact * loadContact(StoragePoint *storagePoint);
	virtual AccountCreateWidget * newCreateAccountWidget(QWidget *parent);
	virtual AccountEditWidget* newEditAccountWidget(Account*, QWidget*);
	virtual ContactWidget * newContactWidget(Contact *contact, QWidget *parent = 0);
	virtual QList<StatusType *> supportedStatusTypes();
	virtual QString idLabel();
	virtual QRegExp idRegularExpression();

	virtual QString name() { return "tlen"; }
	virtual QString displayName() { return "Tlen"; }
	virtual QString iconName() {return dataPath("kadu/modules/data/tlen_protocol/").append("online.png");}
};

#endif // TLEN_PROTOCOL_FACTORY_H
