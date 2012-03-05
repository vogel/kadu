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

#include "exports.h"

class QDomNode;

class CustomProperty;
class StoragePoint;

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class CustomProperties
 * @author Rafal 'Vogel' Malinowski
 * @short Class to store list of custom properites.
 *
 * This class is used as an attachment to any @link Storable @endlink to store any custom properties.
 * Properties can be persistent between program run, but to not have to be. Each loaded property is
 * automatically marked as persistent. It can be changed by calling @link addProperty @endlink
 * with last parameter @link CustomProperties::NonStorable @endlink.
 *
 * This object does not have its own @link StoragePoint @endlink. Loading and storing data is done
 * using  @link loadFrom() @endlink and @link storeTo @endLink methods with @link StoragePoint @endlink as
 * parameter.
 *
 * CustomProperties can import data from old ModuleData classes by mapping values from these object
 * to properties named like this: module-data-name:module-data-property-name. This is also prefferred
 * way to naming new properties: prefix:property-name.
 */
class KADUAPI CustomProperties : public QObject
{
	Q_OBJECT

	QMap<QString, QVariant> Properties;
	QSet<QString> StorableProperties;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Imports data from ModuleData object from given DomNode.
	 * @param node XML node to improt ModuleData data from
	 *
	 * Data from this node is mapped to properties by naming them module-data-name:module-data-property-name.
	 */
	void loadFromModuleData(const QDomNode &node);

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Define if given property should be stored or not.
	 *
	 * Stored properites are persistent between application runs.
	 */
	enum Storability
	{
		/**
		 * Property will be stored and is persistent between application runs.
		 */
		Storable,
		/**
		 * Property will not be stored and is not persistent between application runs.
		 */
		NonStorable
	};

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Create new CustomProperties instance.
	 * @param parent QObject parent of new instance
	 */
	explicit CustomProperties(QObject *parent = 0);
	virtual ~CustomProperties();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Determine if object is worth to be stored.
	 * @return true if object should be stored, defaults to true
	 *
	 * If object as any storable properites, then true is returned.
	 */
	bool shouldStore() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Load custom properties from given storagePoint.
	 * @param storagePoint storage point to load properties from
	 *
	 * Firstly this method reads data from all stored ModuleData nodes, then it removes these nodes.
	 * After that list of values stored in CustomProperty nodes is read. Each read property is marked
	 * as @link CustomProperties::Storable @endlink by default.
	 */
	void loadFrom(const QSharedPointer<StoragePoint> &storagePoint);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Store custom properties to given storagePoint.
	 * @param storagePoint storage point to store properties to
	 *
	 * All properties marked as @link CustomProperties::Storable @endlink are stored to CustomProperty nodes.
	 * All CustomProperty nodes that are not @link CustomProperties::Storable @endlink are removed from
	 * given storagePoint.
	 */
	void storeTo(const QSharedPointer<StoragePoint> &storagePoint) const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Return true if given property is available.
	 * @param name name of property
	 * @return true if given property is available
	 */
	bool hasProperty(const QString &name) const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Add/update property value.
	 * @param name name of property
	 * @param value value of property
	 * @param storability storability parameter of property
	 *
	 * If storability is set to @link CustomProperties::Storable @endlink then added property will
	 * be stored to persistent storage. If not, it will be removed from it.
	 */
	void addProperty(const QString &name, const QVariant &value, Storability storability);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Remove given property from this object and from persistent storage.
	 * @param name name of property
	 */
	void removeProperty(const QString &name);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Read value of property.
	 * @param name name of property
	 * @param defaultValue value returned when property is not available
	 */
	QVariant property(const QString &name, const QVariant &defaultValue) const;

};

/**
 * @}
 */

#endif // CUSTOM_PROPERITES_H
