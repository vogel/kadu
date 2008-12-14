/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_FILE_DATA_MANAGER_H
#define CONFIG_FILE_DATA_MANAGER_H

#include "configuration-window-data-manager.h"

class KADUAPI ConfigFileDataManager : public ConfigurationWindowDataManager
{
public:
	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);
};

#endif 
