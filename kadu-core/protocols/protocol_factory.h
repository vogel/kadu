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

#include "configuration/configuration-window.h"

class AccountData;
class Protocol;

class ProtocolFactory : public QObject
{
public:
	virtual Protocol * newInstance() = 0;
	virtual AccountData * newAccountData() = 0;
	virtual ConfigurationWindow * newConfigurationDialog(AccountData *, QWidget *) = 0;

	virtual QString name() = 0;
	virtual QString displayName() = 0;

	virtual QString iconName()
	{
		return QString::null;
	}

};

#endif // PROTOCOL_FACTORY_H
