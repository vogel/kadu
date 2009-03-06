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

#include "gadu_configuration_dialog.h"
#include "protocols/protocol_factory.h"

class ContactAccountDataWidget;
class GaduContactAccountData;

class GaduProtocolFactory : public ProtocolFactory
{
public:
	virtual Account * newAccount();
	virtual ContactAccountData * newContactAccountData(Contact contact, Account *account, const QString &id);
	virtual ContactAccountData * loadContactAccountData(Contact contact, Account *account);
	virtual ContactAccountDataWidget * newContactAccountDataWidget(ContactAccountData *contactAccountData, QWidget *parent = 0);
	virtual GaduConfigurationDialog * newConfigurationDialog(Account *, QWidget *);

	virtual QString name() { return "gadu"; }
	virtual QString displayName() { return tr("Gadu-Gadu"); }
	virtual QString iconName() { return "BigOnline"; }

};

#endif // GADU_PROTOCOL_FACTORY_H
