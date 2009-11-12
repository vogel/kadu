/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIGURATION_ONTACT_DATA_MANAGER_H
#define CONFIGURATION_ONTACT_DATA_MANAGER_H

#include "configuration-window-data-manager.h"
#include "contacts/contact.h"

class KADUAPI ConfigurationContactDataManager : public ConfigurationWindowDataManager
{
	Contact Data;

protected:
	Contact data() { return Data; }

public:
	explicit ConfigurationContactDataManager(Contact data, QObject *parent = 0);

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};

#endif // CONFIGURATION_ONTACT_DATA_MANAGER_H
