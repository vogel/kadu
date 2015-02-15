/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "exports.h"

#include <QtCore/QVariant>
#include <QtXml/QDomElement>

/**
 * @defgroup Storage
 *
 * This group contains classes and templates that allows to store and load
 * data from XML file. This includes support for Managers (@link<Manager> Manager @endlink
 * and @link<SimpleManager> SimpleManager @endlink) that automates all basic tasks
 * (loading, storing, counting) and @link<Details> Details @endlink that allows to separate data
 * for each object into two different class (one for common data for all instances and one for
 * data specific to given implementation of that class).
 */

/**
 * @addtogroup Storage
 * @{
 */

class ConfigurationApi;

/**
 * @class StoragePoint
 * @author Rafal 'Vogel' Malinowski
 * @short Unique place that can store any data.
 *
 * StoragePoint provides unique place that can store any data
 * Storage point consists of two parts:
 * <ul>
 *   <li>storage - pointer to XML file that contains persistent data</li>
 *   <li>point - DOM node that is parent of all nodes this object uses for storage</li>
 * </ul>
 *
 * Storage points are created in @link<StorableObject> StorableObject @endlink class in
 * @link<createStoragePoint> createStoragePoint @endlink method. You generally should not
 * create instances of StoragePoint outside these classes. Consider derivering from them instead.
 */
class KADUAPI StoragePoint
{
	ConfigurationApi *Storage;
	QDomElement Point;

public:
	StoragePoint(ConfigurationApi *storage, QDomElement point);

	ConfigurationApi * storage() const;
	QDomElement point() const;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads value from XML node (as an attribute).
	 * @param T type of returned value
	 * @param name name of attribute that will be loaded
	 * @return value of XML attribute
	 *
	 * Loads value from XML node as an attribute 'name' with type T.
	 */
template<class T>
	T loadAttribute(const QString &name) const
	{
		QVariant value = Point.attribute(name);
		return value.value<T>();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads value from XML node (as subnode).
	 * @param T type of returned value
	 * @param name name of subnode that will be loaded
	 * @return value of XML subnode
	 *
	 * Loads value from XML node as subnode 'name' with type T.
	 */
template<class T>
	T loadValue(const QString &name) const
	{
		QVariant value;

		if (Storage->hasNode(Point, name))
			value = Storage->getTextNode(Point, name);

		return value.value<T>();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Check if value is available in XML node (as subnode).
	 * @param name name of subnode that will be checked
	 * @return true, if subnode is available
	 *
	 * Check if value is available in XML node (as subnode).
	 */
	bool hasValue(const QString &name) const
	{
		return Storage->hasNode(Point, name);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads value from XML node (as an attribute).
	 * @param T type of returned value
	 * @param name name of attribute that will be loaded
	 * @param def default value, returned when attribute non present
	 * @return value of XML attribute
	 *
	 * Loads value from XML node as an attribute 'name' with type T.
	 * If attribute is non present this method will return value of def.
	 */
template<class T>
	T loadAttribute(const QString &name, T def) const
	{
		if (Point.hasAttribute(name))
		{
			QVariant value = Point.attribute(name);
			return value.value<T>();
		}

		return def;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads value from XML node (as subnode).
	 * @param T type of returned value
	 * @param name name of subnode that will be loaded
	 * @param def default value, returned when subnode non present
	 * @return value of XML subnode
	 *
	 * Loads value from XML node as subnode 'name' with type T.
	 * If subnode is non present this method will return value of def.
	 */
template<class T>
	T loadValue(const QString &name, T def) const
	{
		if (Storage->hasNode(Point, name))
		{
			QVariant value = Storage->getTextNode(Point, name);
			return value.value<T>();
		}

		return def;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores value into XML node (as a subnode).
	 * @param name name of subnode that will store this value
	 * @param value value to be stored
	 *
	 * Stores value into XML node as a subnode 'name' with value 'value'
	 * (value is converted to QString before storing).
	 */
	void storeValue(const QString &name, const QVariant value);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores value into XML node (as an attribute).
	 * @param name name of attribute that will store this value
	 * @param value value to be stored
	 *
	 * Stores value into XML node as a attribute 'name' with value 'value'
	 * (value is converted to QString before storing).
	 */
	void storeAttribute(const QString &name, const QVariant value);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removes value (a subnode) from XML node.
	 * @param name name of subnode that will be removed
	 *
	 * Removes subnode 'name' from XML storage file.
	 */
	void removeValue(const QString &name);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removes value (an attribute) from XML node.
	 * @param name name of attribute that will be removed
	 *
	 * Removes attribute 'name' from XML storage file.
	 */
	void removeAttribute(const QString &name);

};

/**
 * @}
 */
