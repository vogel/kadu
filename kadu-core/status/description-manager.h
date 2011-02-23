/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DESCRIPTION_MANAGER_H
#define DESCRIPTION_MANAGER_H

#include "configuration/configuration-aware-object.h"
#include "storage/storable-string-list.h"

class DescriptionModel;

class KADUAPI DescriptionManager : public QObject, public StorableStringList, private ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(DescriptionManager)

	static DescriptionManager *Instance;

	quint16 MaxNumberOfDescriptions;

	DescriptionManager();
	virtual ~DescriptionManager();

	void import();
	void truncate();

protected:
	virtual void configurationUpdated();

public:
	static DescriptionManager * instance();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();
	virtual QString storageItemNodeName();

	DescriptionModel * model();

	void addDescription(const QString &description);
	void removeDescription(const QString &description);

signals:
	void descriptionAboutToBeAdded(const QString &description);
	void descriptionAdded(const QString &description);
	void descriptionAboutToBeRemoved(const QString &description);
	void descriptionRemoved(const QString &description);

};

#endif // DESCRIPTION_MANAGER_H
