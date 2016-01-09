/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <injeqt/injeqt.h>

class Protocol;
class ProtocolFactory;

class KADUAPI ProtocolsManager : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ProtocolsManager(QObject *parent = nullptr);
	virtual ~ProtocolsManager();

	void registerProtocolFactory(ProtocolFactory *Factory);
	void unregisterProtocolFactory(ProtocolFactory *Factory);

	const QList<ProtocolFactory *> & protocolFactories() const { return Factories; }
	bool hasProtocolFactory(const QString &name);

	int count() { return Factories.count(); }

	ProtocolFactory * byName(const QString &name);
	ProtocolFactory * byIndex(int index);

	int indexOf(ProtocolFactory *protocolFactory) { return Factories.indexOf(protocolFactory); }

signals:
	void protocolFactoryAboutToBeRegistered(ProtocolFactory *factory);
	void protocolFactoryRegistered(ProtocolFactory *factory);
	void protocolFactoryAboutToBeUnregistered(ProtocolFactory *factory);
	void protocolFactoryUnregistered(ProtocolFactory *factory);

private:
	QList<ProtocolFactory *> Factories;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
