/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "storage/storable-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationManager;
class InjectedFactory;
class MobileNumber;

class MobileNumberManager : public StorableObject
{
	Q_OBJECT

	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<InjectedFactory> m_injectedFactory;

	QList<MobileNumber *> Numbers;

protected:
	virtual std::shared_ptr<StoragePoint> createStoragePoint();

	virtual void load();
	virtual void store();

public:
	Q_INVOKABLE explicit MobileNumberManager(QObject *parent = nullptr);
	virtual ~MobileNumberManager();

	void registerNumber(QString number, QString gatewayId);
	void unregisterNumber(QString number);

	virtual QString storageNodeName() { return QStringLiteral("MobileNumbers"); }
	virtual QString storageNodeItemName() { return QStringLiteral("MobileNumber"); }
	virtual StorableObject * storageParent();

	QString gatewayId(const QString &mobileNumber);

private slots:
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
