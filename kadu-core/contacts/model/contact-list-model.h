/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contacts/contact.h"
#include "model/kadu-abstract-model.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Avatars;
class ContactDataExtractor;
struct AvatarId;

/**
 * @addtogroup Contact
 * @{
 */

/**
 * @class ContactListModel
 * @short Model that list contacts.
 *
 * This model lists contacts. Contacts can be added or removed. Model automatically updates itself when a contact is
 * changed.
 */
class ContactListModel : public QAbstractItemModel, public KaduAbstractModel
{
    Q_OBJECT

public:
    /**
     * @short Create new BuddyListModel.
     * @param parent QObject parent of new ContactListModel
     */
    explicit ContactListModel(QObject *parent = nullptr);
    virtual ~ContactListModel();

    /**
     * @short Set list of contacts that this model will represent.
     * @param contacts new list of contacts
     *
     * All current contacts are disconnected and removed from list and new ones are added and connected.
     * List can be later changed by @link addContact @endlink and @link removeContact @endlink methods.
     */
    void setContactList(const QVector<Contact> &contacts);

    /**
     * @short Add new contact to list.
     * @param contact new contact to add
     *
     * Add contact to list and connect it to this model. If contact is already on the list nothing is done.
     */
    void addContact(const Contact &contact);

    /**
     * @short Remove contact from list.
     * @param contact contact to remove
     *
     * Remove contact from list and disconnect it to this model. If contact is not on the list nothing is done.
     */
    void removeContact(const Contact &contact);

    /**
     * @short Return index for given row, column and parent index.
     * @param row row of index
     * @param column column of index
     * @param parent parent index of new index
     * @return index for given row, column and parent index
     *
     * This method returns index for given coordinates. Returned index's InternalPoitner property points to
     * @link ContactShared @endlink.
     */
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    /**
     * @short Return parent index for given index.
     * @param child child to get parent of
     * @return parent index for given index
     *
     * This method always returns invalid index.
     */
    virtual QModelIndex parent(const QModelIndex &child) const;

    /**
     * @short Return column count for given index.
     * @param parent parent index
     * @return column count
     *
     * This method always returns 1.
     */
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @short Return row count for given index.
     * @param parent parent index
     * @return row count
     *
     * This method returns number of items in contact list if parent is not valid and zero otherwise.
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @short Return data for given index and role.
     * @param index model index
     * @param role role of data to return
     * @return data for given index and role
     *
     * This method returns data for given index and role. Class @link ContactDataExtractor @endlink is used to
     * extract data from @link Contact @endlink. If possible, Contact's owner @link Buddy @endlink data is used.
     */
    virtual QVariant data(const QModelIndex &index, int role) const;

    /**
     * @todo Find out why it does not return anything for Contact values.
     * @short Return list of indexes for given value.
     * @param value value to search in model
     * @return list of indexes for given value
     *
     * This method works only for value of type @link Buddy @endlink. Each @link Contact @endlink with given
     * owner @link Buddy @endlink is returned.
     */
    virtual QModelIndexList indexListForValue(const QVariant &value) const;

private:
    QPointer<ContactDataExtractor> m_contactDataExtractor;

    QVector<Contact> m_list;

    void avatarUpdated(const AvatarId &id);

    /**
     * @short Connect given contact to this object's slots.
     * @param contact contact to connect
     *
     * Calling this method on each new contact ensures that model is automatically updated.
     */
    void connectContact(const Contact &contact);

    /**
     * @short Disonnect given contact from this object's slots.
     * @param contact contact to disconnect
     *
     * Calling this method on each removed contact ensures that this model does not unneccessary updates.
     */
    void disconnectContact(const Contact &contact);

private slots:
    INJEQT_SET void setAvatars(Avatars *avatars);
    INJEQT_SET void setContactDataExtractor(ContactDataExtractor *contactDataExtractor);

    /**
     * @short Slot called every time a contact's data changes.
     *
     * This slot is connected to every contact in list. Updated contact is read from QObject::sender() method.
     */
    void contactUpdated();
};

/**
 * @}
 */
