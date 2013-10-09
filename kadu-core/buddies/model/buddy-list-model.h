/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef BUDDY_LIST_MODEL_H
#define BUDDY_LIST_MODEL_H

#include <QtCore/QModelIndex>

#include "accounts/accounts-aware-object.h"

#include "buddies/buddy-list.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "status/status.h"
#include "exports.h"

#include "accounts/accounts-aware-object.h"
#include "model/kadu-abstract-model.h"

class Buddy;

/**
 * @addtogroup Buddy
 * @{
 */

/**
 * @class BuddyListModel
 * @author Rafał 'Vogel' Malinowski
 * @short Model that list buddies with their contacts.
 *
 * This model lists buddies with their contacts. Buddies can be added or removed. Model automatically updates
 * itself when a buddy is changed, a buddy's contact is changed or a list of buddy's contacts is changed.
 */
class KADUAPI BuddyListModel : public QAbstractItemModel, public KaduAbstractModel, public AccountsAwareObject
{
	Q_OBJECT

	bool Checkable;
	BuddyList List;
	BuddySet CheckedBuddies;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Converts variant to Buddy.
	 * @param variant variant to convert
	 * @return Buddy from given variant.
	 *
	 * This class can convert variants that holds a Buddy instance or a Talkable instance that is a valid Buddy.
	 */
	Buddy buddyFromVariant(const QVariant &variant) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Converts variant to Contact.
	 * @param variant variant to convert
	 * @return Contact from given variant.
	 *
	 * This class can convert variants that holds a Contact instance or a Talkable instance that is a valid Contact.
	 */
	Contact contactFromVariant(const QVariant &variant) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @todo Remove after Qt 4.8 is required
	 * @short Return true if given index is checkable.
	 * @param index index to check
	 * @return true if given index is checkable
	 *
	 * Index is checkable when whole model is checkable and given index holds a valid Buddy.
	 */
	bool isCheckableIndex(const QModelIndex &index) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return contact with given contactIndex from buddy holded at given index.
	 * @param index model index of buddy
	 * @param contactIndex index of contact in given buddy
	 * @return contact with given contactIndex from buddy holded at given index
	 *
	 * If index does not hold a valid buddy or contactIndex is out of range then null Contact is returned.
	 */
	Contact buddyContact(const QModelIndex &index, int contactIndex) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Connect given buddy to this object's slots.
	 * @param buddy buddy to connect
	 *
	 * Calling this method on each new buddy ensures that model is automatically updated.
	 */
	void connectBuddy(const Buddy &buddy);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Disonnect given buddy from this object's slots.
	 * @param buddy buddy to disconnect
	 *
	 * Calling this method on each removed buddy ensures that this model does not unneccessary updates.
	 */
	void disconnectBuddy(const Buddy &buddy);

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called every time a buddy's data changes.
	 *
	 * This slot is connected to every buddy in list. Updated buddy is read from QObject::sender() method.
	 */
	void buddyUpdated();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called every time a contact's data changes.
	 * @param contact updated Contact
	 *
	 * This slot is connected to @link ContactManager @endlink singleton. When updated contact belongs to one of
	 * buddy on the list then an update is performed.
	 */
	void contactUpdated(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called every time a contact's status changes.
	 * @param contact updated Contact
	 * @param oldStatus old status of updated Contact
	 *
	 * This slot is connected to every registered @link Addoun @endlink. When updated contact belongs to one of
	 * buddy on the list then an update is performed.
	 */
	void contactStatusChanged(const Contact &contact, const Status &oldStatus);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called every time a contact is about to be added to known buddy.
	 * @param contact about-to-be-added contact
	 *
	 * This slot is connected to every buddy in list. Updated buddy is read from QObject::sender() method.
	 */
	void contactAboutToBeAdded(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called every time a contact is added to known buddy.
	 * @param contact newly added contact
	 *
	 * This slot is connected to every buddy in list. Updated buddy is read from QObject::sender() method.
	 */
	void contactAdded(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called every time a contact is about to be removed from known buddy.
	 * @param contact about-to-be-removed contact
	 *
	 * This slot is connected to every buddy in list. Updated buddy is read from QObject::sender() method.
	 */
	void contactAboutToBeRemoved(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called every time a contact is removed from known buddy.
	 * @param contact just removed contact
	 *
	 * This slot is connected to every buddy in list. Updated buddy is read from QObject::sender() method.
	 */
	void contactRemoved(const Contact &contact);

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called every time a new account is registerd.
	 * @param account just registered account
	 *
	 * In this method a connection is made to ensure that this model is informed about status changes of contacts.
	 */
	virtual void accountRegistered(Account account);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called every time an account is unregisterd.
	 * @param account just unregistered account
	 *
	 * In this method connection connection made in @link accountRegistered @endlink is disconnected.
	 */
	virtual void accountUnregistered(Account account);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new BuddyListModel.
	 * @param parent QObject parent of new BuddyListModel
	 *
	 * In this contructor connection to @link ContactManager @endlink is made to ensure that this model
	 * is informed about changes in contacts' data.
	 */
	explicit BuddyListModel(QObject *parent = 0);
	virtual ~BuddyListModel();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set list of buddies that this model will represent.
	 * @param list new list of buddies
	 *
	 * All current buddies are disconnected and removed from list and new ones are added and connected.
	 * List can be later changed by @link addBuddy @endlink and @link removeBuddy @endlink methods.
	 */
	void setBuddyList(const BuddyList &list);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Add new buddy to list.
	 * @param buddy new buddy to add
	 *
	 * Add buddy to list and connect it to this model. If buddy is already on the list nothing is done.
	 */
	void addBuddy(const Buddy &buddy);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Remove buddy from list.
	 * @param buddy buddy to remove
	 *
	 * Remove buddy from list and disconnect it to this model. If buddy is not on the list nothing is done.
	 */
	void removeBuddy(const Buddy &buddy);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @todo Remove after Qt 4.8 is required
	 * @short Set if the list should be checkable.
	 * @param checkable new value of checkable property
	 *
	 * If list is checkable then list of checked buddies can be retreived by calling @link checkedBuddies @endlink
	 * method. On every item these will be checkbox displayed next to buddy. Signal @link checkedBuddiesChanged
	 * @endlink may be emited after calling this method.
	 */
	void setCheckable(bool checkable);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return index for given row, column and parent index.
	 * @param row row of index
	 * @param column column of index
	 * @param parent parent index of new index
	 * @return index for given row, column and parent index
	 *
	 * This method returns index for given coordinates. Returned index's InternalPoitner property points to
	 * @link BuddyShared @endlink or @link ContactShared @endlink instance, so it is eady to retreive data later.
	 */
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return parent index for given index.
	 * @param child child to get parent of
	 * @return parent index for given index
	 *
	 * This method returns parent index for given child index. If child index contains @link BuddyShared @endlink
	 * pointer then it has no parent index. If child index contains @link ContactShared @endlink pointer then
	 * an OwnerBuddy of this @link Contact @endlink is used to compute parent index's coordinates and return it.
	 */
	virtual QModelIndex parent(const QModelIndex &child) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return column count for given index.
	 * @param parent parent index
	 * @return column count
	 *
	 * This method always returns 1.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return row count for given index.
	 * @param parent parent index
	 * @return row count
	 *
	 * This method returns number of items in buddy list if parent is not valid, number of contacts of buddy is
	 * parent is a buddy index, and 0 if parent is contact index.
	 */
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return flags for given index.
	 * @param index model index
	 * @return flags for given index
	 *
	 * This method returns flags for given index. It adds Qt:;ItemIsDragEnabled for every valid index and
	 * Qt::ItemIsUserCheckable if model's Checkable property is set to true.
	 */
	virtual QFlags<Qt::ItemFlag> flags(const QModelIndex &index) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return data for given index and role.
	 * @param index model index
	 * @param role role of data to return
	 * @return data for given index and role
	 *
	 * This method returns data for given index and role. For buddy indexes data is extraced with @link ContactDataExtractor
	 * @endlink for this buddy's preferred contact when there is such contact and role is not equal to TalkableRole. In
	 * other cases @link BuddyDataExtractor @endlink is used.
	 *
	 * For contact indexes @link ContactDataExtractor @endlink is used.
	 */
	virtual QVariant data(const QModelIndex &index, int role) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @todo Remove after Qt 4.8 is required
	 * @short Set Checked property on given buddy.
	 * @param index model index
	 * @param value new vaue
	 * @param role role of data to change
	 *
	 * Only Qt::CheckStateRole is valid and only when Checkable property is true. Only buddy indexes are acceptable
	 * as index parameter. After successfull change @link checkedBuddiesChanged @endlink signal is emited.
	 */
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return list of mime types supported by this model.
	 * @return list of mime types supported by this model
	 *
	 * See @link BuddyListMimeDataHelper @endlink for more info on buddy list mime types.
	 */
	virtual QStringList mimeTypes() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return mime data for given indexes.
	 * @param indexes list of indexes to convert to QMimeData
	 * @return mime data for given indexes
	 *
	 * See @link BuddyListMimeDataHelper @endlink for more info on buddy list mime types.
	 */
	virtual QMimeData * mimeData(const QModelIndexList &indexes) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @todo Remove after Qt 4.8 is required
	 * @short Return set of checked buddies.
	 * @return set of checked buddies
	 *
	 * Return set of checked buddies. If Checkable property is false empty set is returned.
	 */
	BuddySet checkedBuddies() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return list of indexes for given value.
	 * @param value value to search in model
	 * @return list of indexes for given value
	 *
	 * This method works only for value of type @link Talkable @endlink, @link Buddy @endlink or @link Contact @endlink.
	 * If type is @link Talkable @endlink then value is converted to @link Buddy @endlink or @link Contact @endlink first.
	 * Then given item is located on model and proper list of one or zero indexes is returned.
	 */
	virtual QModelIndexList indexListForValue(const QVariant &value) const;


signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @todo Remove after Qt 4.8 is required
	 * @short Signal emited when set of checked buddies changed.
	 * @param checkedBuddies new set of checke buddies
	 */
	void checkedBuddiesChanged(const BuddySet &checkedBuddies);

};

/**
 * @}
 */

#endif // BUDDY_LIST_MODEL_H
