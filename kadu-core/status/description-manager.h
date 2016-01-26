/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "configuration/configuration-aware-object.h"
#include "storage/storable-string-list.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationManager;
class Configuration;
class DescriptionModel;

class KADUAPI DescriptionManager : public StorableStringList, private ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit DescriptionManager(QObject *parent = nullptr);
	virtual ~DescriptionManager();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();
	virtual QString storageItemNodeName();

	DescriptionModel * model();

	void addDescription(const QString &description);
	void removeDescription(const QString &description);
	void clearDescriptions();

signals:
	void descriptionAboutToBeAdded(const QString &description);
	void descriptionAdded(const QString &description);
	void descriptionAboutToBeRemoved(const QString &description);
	void descriptionRemoved(const QString &description);

protected:
	virtual void configurationUpdated();

private:
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<Configuration> m_configuration;

	int MaxNumberOfDescriptions;

	void import();
	void truncate();

private slots:
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
