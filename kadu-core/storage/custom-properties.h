/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CUSTOM_PROPERITES_H
#define CUSTOM_PROPERITES_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QSharedPointer>

class QDomNode;

class CustomProperty;
class StoragePoint;

class CustomProperties : public QObject
{
	Q_OBJECT

	QMap<QString, QVariant> Properties;
	QSet<QString> StorableProperties;

	void loadFromModuleData(const QDomNode &node);

public:
	enum Storability
	{
		Storable,
		NonStorable
	};

	explicit CustomProperties(QObject *parent = 0);
	virtual ~CustomProperties();

	bool shouldStore() const;
	void loadFrom(const QSharedPointer<StoragePoint> &storagePoint);
	void storeTo(const QSharedPointer<StoragePoint> &storagePoint) const;

	bool hasProperty(const QString &name);

	void addProperty(const QString &name, const QVariant &value, Storability storability);
	void removeProperty(const QString &name);
	QVariant property(const QString &name, const QVariant &defaultValue);

};

#endif // CUSTOM_PROPERITES_H
