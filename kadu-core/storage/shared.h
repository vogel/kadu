/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QSharedData>

#include "exports.h"
#include "misc/change-notifier.h"
#include "storage/uuid-storable-object.h"

/**
 * @addtogroup Storage
 * @{
 */

#define KaduShared_PropertyRead_M(type, name) \
    type name()                               \
    {                                         \
        ensureLoaded();                       \
        return m_##name;                      \
    }

#define KaduShared_PropertyWrite_M(type, name, capitalized_name) \
    void set##capitalized_name(type name)                        \
    {                                                            \
        ensureLoaded();                                          \
        if (m_##name != name)                                    \
        {                                                        \
            m_##name = name;                                     \
            changeNotifier().notify();                           \
        }                                                        \
    }

#define KaduShared_Property_M(type, name, capitalized_name) \
    KaduShared_PropertyRead_M(type, name) KaduShared_PropertyWrite_M(type, name, capitalized_name)
/**
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
    type name()                                               \
    {                                                         \
        ensureLoaded();                                       \
        return capitalized_name;                              \
    }

/**
 * @short Defines setter for given property of StorableObject.
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property (setter will be called set##capitalized_name)
 *
 * Defines setter for given property of @link StorableObject @endlink. Value of property will not be
 * ovveriden by load, because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Setter is called set##capitalized_name).
 * Calls @link changeNotifier().notify @endlink method after data is changed.
 */
#define KaduShared_PropertyWrite(type, name, capitalized_name) \
    void set##capitalized_name(type name)                      \
    {                                                          \
        ensureLoaded();                                        \
        if (capitalized_name != name)                          \
        {                                                      \
            capitalized_name = name;                           \
            changeNotifier().notify();                         \
        }                                                      \
    }

/**
 * @short Defines getter and setter for given property of StorableObject.
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property.
 *
 * Defines both getter and setter for given property of @link StorableObject @endlink. Value of property will
 * always be valid, because these methods calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called name, setter is called set##capitalized_name).
 * Setter calls @link changeNotifier().notify @endlink method after data is changed.
 */
#define KaduShared_Property(type, name, capitalized_name) \
    KaduShared_PropertyRead(type, name, capitalized_name) KaduShared_PropertyWrite(type, name, capitalized_name)

/**
 * @short Defines getter for given boolean property of StorableObject.
 * @param capitalized_name name of property
 *
 * Defines getter for given property of @link StorableObject @endlink. Value of property is always
 * valid because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called is##capitalized_name.
 */
#define KaduShared_PropertyBoolRead(capitalized_name) \
    bool is##capitalized_name()                       \
    {                                                 \
        ensureLoaded();                               \
        return capitalized_name;                      \
    }

/**
 * @short Defines setter for given boolean property of StorableObject.
 * @param capitalized_name name of property (setter will be called set##capitalized_name)
 *
 * Defines setter for given property of @link StorableObject @endlink. Value of property will not be
 * ovveriden by load, because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Setter is called set##capitalized_name).
 * Calls @link changeNotifier().notify @endlink method after data is changed.
 */
#define KaduShared_PropertyBoolWrite(capitalized_name) \
    void set##capitalized_name(bool name)              \
    {                                                  \
        ensureLoaded();                                \
        if (capitalized_name != name)                  \
        {                                              \
            capitalized_name = name;                   \
            changeNotifier().notify();                 \
        }                                              \
    }

/**
 * @short Defines getter and setter for given boolean property of StorableObject.
 * @param capitalized_name name of property.
 *
 * Defines both getter and setter for given property of @link StorableObject @endlink. Value of property will
 * always be valid, because these methods calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called is##capitlized_name, setter is called
 * set##capitalized_name).
 * Setter calls @link changeNotifier().notify @endlink method after data is changed.
 */
#define KaduShared_PropertyBool(capitalized_name) \
    KaduShared_PropertyBoolRead(capitalized_name) KaduShared_PropertyBoolWrite(capitalized_name)

/**
 * @short Declares getter for given property of StorableObject.
 * @param type type of property
 * @param name name of getter
 *
 * Declares getter for given property of @link StorableObject @endlink.
 */
#define KaduShared_PropertyReadDecl(type, name) type name();

/**
 * @short Declares setter for given property of StorableObject.
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property (setter will be called set##capitalized_name)
 *
 * Declares setter for given property of @link StorableObject @endlink. Setter is called set##capitalized_name).
 */
#define KaduShared_PropertyWriteDecl(type, name, capitalized_name) void set##capitalized_name(type name);

/**
 * @short Declares getter and setter for given property of StorableObject.
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property.
 *
 * Declares both getter and setter for given property of @link StorableObject @endlink.
 * Getter is called name, setter is called set##capitalized_name).
 * Argument of the setter will be a const reference to 'type'.
 */
#define KaduShared_PropertyDeclCRW(type, name, capitalized_name) \
    KaduShared_PropertyReadDecl(type, name) KaduShared_PropertyWriteDecl(const type &, name, capitalized_name)

/**
 * @short Defines getter for given pointed property of StorableObject.
 * @param class_name class name
 * @param type type of property
 * @param name name of getter
 * @param capitalized_name name of property
 *
 * Defines getter for given pointed property of @link StorableObject @endlink. Value of property is always
 * valid because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called name.
 */
#define KaduShared_PropertyPtrReadDef(class_name, type, name, capitalized_name) \
    type class_name::name()                                                     \
    {                                                                           \
        ensureLoaded();                                                         \
        return *capitalized_name;                                               \
    }

/**
 * @short Defines setter for given pointed property of StorableObject.
 * @param class_name class name
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property (setter will be called set##capitalized_name)
 *
 * Defines setter for given pointed property of @link StorableObject @endlink. Value of property will not be
 * ovveriden by load, because this method calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Setter is called set##capitalized_name).
 * Calls @link changeNotifier().notify @endlink method after data is changed.
 */
#define KaduShared_PropertyPtrWriteDef(class_name, type, name, capitalized_name) \
    void class_name::set##capitalized_name(type name)                            \
    {                                                                            \
        ensureLoaded();                                                          \
        if (*capitalized_name != name)                                           \
        {                                                                        \
            *capitalized_name = name;                                            \
            changeNotifier().notify();                                           \
        }                                                                        \
    }

/**
 * @short Defines getter and setter for given pointed property of StorableObject.
 * @param class_name class name
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property.
 *
 * Defines both getter and setter for given pointed property of @link StorableObject @endlink. Value of property will
 * always be valid, because these methods calls @link<StorableObject::ensureLoaded @endLink, so object is always
 * fully loaded before this method returns. Getter is called name, setter is called set##capitalized_name).
 * Setter calls @link changeNotifier().notify @endlink method after data is changed.
 * Argument of the setter will be a const reference to 'type'.
 */
#define KaduShared_PropertyPtrDefCRW(class_name, type, name, capitalized_name) \
    KaduShared_PropertyPtrReadDef(class_name, type, name, capitalized_name)    \
        KaduShared_PropertyPtrWriteDef(class_name, const type &, name, capitalized_name)

#define KaduShared_PropertyPtrReadDef_M(class_name, type, name) \
    type class_name::name()                                     \
    {                                                           \
        ensureLoaded();                                         \
        return *m_##name;                                       \
    }

#define KaduShared_PropertyPtrWriteDef_M(class_name, type, name, capitalized_name) \
    void class_name::set##capitalized_name(type name)                              \
    {                                                                              \
        ensureLoaded();                                                            \
        if (*m_##name != name)                                                     \
        {                                                                          \
            *m_##name = name;                                                      \
            changeNotifier().notify();                                             \
        }                                                                          \
    }

#define KaduShared_PropertyPtrDefCRW_M(class_name, type, name, capitalized_name) \
    KaduShared_PropertyPtrReadDef_M(class_name, type, name)                      \
        KaduShared_PropertyPtrWriteDef_M(class_name, const type &, name, capitalized_name)

/**
 * @class Shared
 * @short UuidStorableObject that acts like QSharedData - can be used as data for many SharedBase object.
 *
 * This class stores data for @link SharedBase @endlink objects that allows many places of code to access
 * data of one object without concerning about copying, creating or destroying object. Many SharedBase objects
 * can contain the same Shared object (so the same data) - especially copied objects, so every change
 * in one of them is immediately visible in all other. Setter
 *
 * Note that because of QExplicitlySharedDataPointer implementation details it is not guaranteed that in
 * some method when you do something causing directly or not to remove the last @link SharedBase @endlink
 * object that refers to this object, this object will be deleted after your method ends. Actually, the object
 * may be deleted in the middle of it, so when such last reference loss is possible in a method, you need
 * to create a @link SharedBase @endlink guard object on the stack that will delay deletion to the moment
 * when the stack unrolls and destroys that @link SharedBase @endlink object or even avoid deletion at all
 * if after removing the last reference you set up another one.
 *
 * This class holds instance of @link ChangeNotifier @endlink class to handle notifications about updated.
 */
class KADUAPI Shared : public UuidStorableObject, public QSharedData
{
    ChangeNotifier MyChangeNotifier;

protected:
    virtual void load();
    virtual void store();

public:
    explicit Shared(const QUuid &uuid, QObject *parent = nullptr);
    virtual ~Shared();

    void loadStub();

    virtual void aboutToBeRemoved();

    /**
     * @short Return ChangeNotifier for this object.
     * @return ChangeNotifier instance for this object
     *
     * This method never returns null value.
     */
    ChangeNotifier &changeNotifier();
};

/**
 * @}
 */

#endif   // SHARED_H
