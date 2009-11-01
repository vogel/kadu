/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_DATA_MANAGER_H
#define BUDDY_DATA_MANAGER_H

#include "configuration/configuration-window-data-manager.h"
#include "buddies/buddy.h"

class BuddyDataManager : public ConfigurationWindowDataManager
{
	Q_OBJECT

	Buddy MyBuddy;

public:
	BuddyDataManager(Buddy buddy, QObject *parent = 0);

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};

#endif // BUDDY_DATA_MANAGER_H
