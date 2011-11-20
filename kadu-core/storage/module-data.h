/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MODULE_DATA_H
#define MODULE_DATA_H

#include <QtCore/QUuid>

#include "named-storable-object.h"

#include "exports.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class ModuleData
 * @author Rafal 'Vogel' Malinowski
 * @short Object that can load itself from XML file and store data there. Object is identified by module name, node name is fixed.
 *
 * Many objects of this class can be stored under one master XML node in storage.
 * Each object is identified by unique name. Every object is stored in node named "ModuleData"
 * with attribute 'name'.
 *
 * ModuleData object are assigned to normal @link<StoragePoint> StoragePoint @endlink object.
 * This class represents data that every plugin can add to all important objects in Kadu.
 */
class KADUAPI ModuleData : public QObject, public NamedStorableObject
{
	Q_OBJECT

	QString ModuleName;
	StorableObject *CurrentStorageParent;

public:
	ModuleData(const QString &moduleName, StorableObject *storageParent, QObject *qobjectParent);
	virtual ~ModuleData();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

};

/**
 * @}
 */

#endif // MODULE_DATA_H
