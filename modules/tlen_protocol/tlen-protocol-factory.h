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

class TlenProtocolFactory : public ProtocolFactory
{
	Q_DISABLE_COPY(TlenProtocolFactory)

	static TlenProtocolFactory *Instance;
	QList<StatusType *> SupportedStatusTypes;
	QRegExp IdRegularExpression;

	TlenProtocolFactory();

public:
	static TlenProtocolFactory * instance();

	virtual Protocol * createProtocolHandler(Account account);
	virtual AccountDetails * createAccountDetails(AccountShared *accountShared);
	virtual ContactDetails * createContactDetails(ContactShared *contactShared);
	virtual AccountAddWidget * newAddAccountWidget(QWidget *parent = 0) { return 0; }
	virtual AccountCreateWidget * newCreateAccountWidget(QWidget *parent);
	virtual AccountEditWidget* newEditAccountWidget(Account, QWidget*);
	virtual ContactWidget * newContactWidget(Contact contact, QWidget *parent = 0);

	virtual QList<StatusType *> supportedStatusTypes();
	virtual QString idLabel();
	virtual QRegExp idRegularExpression();

	virtual QString name() { return "tlen"; }
	virtual QString displayName() { return "Tlen"; }
	virtual QString iconName();
};

#endif // TLEN_PROTOCOL_FACTORY_H
