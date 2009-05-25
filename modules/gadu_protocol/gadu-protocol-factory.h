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

#include "gadu-configuration-dialog.h"
#include "protocols/protocol-factory.h"

class ContactAccountDataWidget;
class GaduContactAccountData;

class GaduProtocolFactory : public ProtocolFactory
{
	Q_DISABLE_COPY(GaduProtocolFactory)

	static GaduProtocolFactory *Instance;

	GaduProtocolFactory();

public:
	static GaduProtocolFactory * instance();

	virtual Account * newAccount();
	virtual Account * loadAccount(StoragePoint *accountStoragePoint);
	virtual ContactAccountData * newContactAccountData(Contact contact, Account *account, const QString &id, bool loadFromConfiguration = true);
	virtual ContactAccountData * loadContactAccountData(Contact contact, Account *account);
	virtual AccountCreateWidget * newCreateAccountWidget(QWidget *parent);
	virtual AccountEditWidget * newEditAccountWidget(Account *, QWidget *parent);
	virtual ContactAccountDataWidget * newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent = 0);
	virtual GaduConfigurationDialog * newConfigurationDialog(Account *, QWidget *);

	virtual QString name() { return "gadu"; }
	virtual QString displayName() { return tr("Gadu-Gadu"); }
	virtual QString iconName() { return "BigOnline"; }

};

#endif // GADU_PROTOCOL_FACTORY_H
