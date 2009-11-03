/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_PROTOCOL_FACTORY_H
#define GADU_PROTOCOL_FACTORY_H

#include "protocols/protocol-factory.h"

class ContactAccountDataWidget;
class GaduContactAccountData;

class GaduProtocolFactory : public ProtocolFactory
{
	Q_DISABLE_COPY(GaduProtocolFactory)

	static GaduProtocolFactory *Instance;
	QList<StatusType *> SupportedStatusTypes;
	QRegExp IdRegularExpression;

	GaduProtocolFactory();

public:
	static GaduProtocolFactory * instance();

	virtual Protocol * createProtocolHandler(Account account);
	virtual AccountDetails * createAccountDetails(Account account);
	virtual ContactAccountData * newContactAccountData(Account account, Buddy buddy, const QString &id);
	virtual ContactAccountData * loadContactAccountData(StoragePoint *storagePoint);
	virtual AccountCreateWidget * newCreateAccountWidget(QWidget *parent);
	virtual AccountEditWidget * newEditAccountWidget(Account, QWidget *parent);
	virtual ContactAccountDataWidget * newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent = 0);
	virtual QList<StatusType *> supportedStatusTypes();
	virtual QString idLabel();
	virtual QRegExp idRegularExpression();

	virtual QString name() { return "gadu"; }
	virtual QString displayName() { return tr("Gadu-Gadu"); }
	virtual QString iconName() { return "BigOnline"; }

};

#endif // GADU_PROTOCOL_FACTORY_H
