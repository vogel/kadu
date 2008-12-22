/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QPushButton>

#include "gadu.h"
#include "gadu_account_data.h"
#include "gadu_configuration_dialog.h"

#include "gadu_protocol_factory.h"

Protocol * GaduProtocolFactory::newInstance()
{
	return new GaduProtocol(0, this);
}

AccountData * GaduProtocolFactory::newAccountData()
{
	return new GaduAccountData();
}

GaduConfigurationDialog * GaduProtocolFactory::newConfigurationDialog(AccountData *accountData, QWidget *parent)
{
	GaduAccountData *gaduAccountData = dynamic_cast<GaduAccountData *>(accountData);

	return 0 != gaduAccountData
		? new GaduConfigurationDialog(gaduAccountData, parent)
		: 0;
}
