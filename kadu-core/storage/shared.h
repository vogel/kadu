/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SHARED_H
#define SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>

#include "status/status.h"

#include "storage/uuid-storable-object.h"
#include "exports.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines getter for given property of StorableObject.
 * @param type type of property
 * @param name name of getter
 * @param capitalized_name name of property
 *
 * Defines getter for given property of @link StorableObject @endlink. Value of property is always
 * valid because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called name.
 */
#define KaduShared_PropertyRead(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; }

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines setter for given property of StorableObject.
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property (setter will be called set##capitalized_name)
 *
 * Defines setter for given property of @link StorableObject @endlink. Value of property will not be
 * ovveriden by load, because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Setter is called set##capitalized_name).
 * Calls @link dataUpdated @endlink method after data is changed.
 */
#define KaduShared_PropertyWrite(type, name, capitalized_name) \
	void set##capitalized_name(type name) { ensureLoaded(); if (capitalized_name != name) { capitalized_name = name; dataUpdated(); } }

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines getter and setter for given property of StorableObject.
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property.
 *
 * Defines both getter and setter for given property of @link StorableObject @endlink. Value of property will
 * always be valid, because these methods calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called name, setter is called set##capitalized_name).
 * Setter calls @link dataUpdated @endlink method after data is changed.
 */
#define KaduShared_Property(type, name, capitalized_name) \
	KaduShared_PropertyRead(type, name, capitalized_name) \
	KaduShared_PropertyWrite(type, name, capitalized_name)


/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines getter for given boolean property of StorableObject.
 * @param capitalized_name name of property
 *
 * Defines getter for given property of @link StorableObject @endlink. Value of property is always
 * valid because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called is##capitalized_name.
 */
#define KaduShared_PropertyBoolRead(capitalized_name) \
	bool is##capitalized_name() { ensureLoaded(); return capitalized_name; }

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines setter for given boolean property of StorableObject.
 * @param capitalized_name name of property (setter will be called set##capitalized_name)
 *
 * Defines setter for given property of @link StorableObject @endlink. Value of property will not be
 * ovveriden by load, because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Setter is called set##capitalized_name).
 * Calls @link dataUpdated @endlink method after data is changed.
 */
#define KaduShared_PropertyBoolWrite(capitalized_name) \
	void set##capitalized_name(bool name) { ensureLoaded(); if (capitalized_name != name) { capitalized_name = name; dataUpdated(); } }

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines getter and setter for given boolean property of StorableObject.
 * @param capitalized_name name of property.
 *
 * Defines both getter and setter for given property of @link StorableObject @endlink. Value of property will
 * always be valid, because these methods calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called is##capitlized_name, setter is called set##capitalized_name).
 * Setter calls @link dataUpdated @endlink method after data is changed.
 */
#define KaduShared_PropertyBool(capitalized_name) \
	KaduShared_PropertyBoolRead(capitalized_name) \
	KaduShared_PropertyBoolWrite(capitalized_name)

/**
 * @author Rafal 'Vogel' Malinowski
 * @class Shared
 * @short UuidStorableObject that acts like QSharedData - can be used as data for many SharedBase object.
 *
 * This class stores data for @link SharedBase @endlink objects that allows many places of code to access
 * data of one object without concerning about copying, creating or destroying object. Many SharedBase objects
 * can contain the same Shared object (so the same data) - especially copied objects, so every change
 * in one of them is immediately visible in all other. Setter
 *
 * Support for notyfiing of any changes to this object is also available. Every change calls
 * @link dataUpdated @endlink method (that can be overridden and used to send a signal). To block
 * this method (when multiple changes are performed at once) @link blockUpdatedSignal @endlink
 * can be called. Then first change creates deffered call that is executed after call of
 * @link unblockUpdatedSignal @endlink (multiple calls of blockUpdatedSignal requires as much calls
 * of unblockUpdatedSignal to call doEmitUpdated).
 *
 * Note that because of QExplicitlySharedDataPointer implementation details it is not guaranteed that in
 * some method when you do something causing directly or not to remove the last @link SharedBase @endlink
 * object that refers to this object, this object will be deleted after your method ends. Actually, the object
 * may be deleted in the middle of it, so when such last reference loss is possible in a method, you need
 * to create a @link SharedBase @endlink guard object on the stack that will delay deletion to the moment
 * when the stack unrolls and destroys that @link SharedBase @endlink object or even avoid deletion at all
 * if after removing the last reference you set up another one.
 */
class KADUAPI Shared : public UuidStorableObject, public QSharedData
{
	int BlockUpdatedSignalCount;
	bool Updated;

	void doEmitUpdated();

protected:
	virtual void load();
	void loadStub();

	void dataUpdated();
	virtual void emitUpdated();

public:
	explicit Shared(QUuid Uuid);
	virtual ~Shared();

	virtual void store();
	virtual void aboutToBeRemoved();

	void blockUpdatedSignal();
	void unblockUpdatedSignal();

};

/**
 * @}
 */

#endif // SHARED_H
