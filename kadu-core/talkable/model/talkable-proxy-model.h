/*
 * %kadu copyright begin%
 * %kadu copyright end%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef TALKABLE_PROXY_MODEL_H
#define TALKABLE_PROXY_MODEL_H

#include <QtGui/QSortFilterProxyModel>

#include "exports.h"

class Buddy;
class Chat;
class Contact;
class TalkableFilter;

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class TalkableProxyModel
 * @author Rafał 'Vogel' Malinowski
 * @short Proxy model that can sort and filter items of Chat, Buddy or Contact type.
 *
 * This proxy model can be used to filter and sort models that contains only items of Chat, Buddy or Contact type.
 * Q_ASSERT is used to enforce this constraint.
 *
 * Filtering is done TalkableFilters objects that can be easily added to instanced of this proxy model. Each filter
 * can accept or reject any item or postpone decision to next filter. Only top-level items are filtered - if top-level
 * item is accepted then whole subtree of this item is accepted as well.
 *
 * Sorting is hardcoded for now - it will change in future - new class TalkableComparator will be created to allow
 * for pluggable sorting mechanism. The only way to alter soring is to use setSortByStatus() to enable or disable
 * sorting by status.
 *
 * In current implementation chats are displayed on top, then buddies, then contacts. In each group different sorting
 * methods are used. Refer to documentation of lessThan() methods for details.
 */
class KADUAPI TalkableProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	QList<TalkableFilter *> TalkableFilters;

	bool BrokenStringCompare;
	bool SortByStatusAndUnreadMessages;
	bool lessThan(const QString &left, const QString &right) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Compares two Chats instances.
	 * @param left Chat to compare
	 * @param right Chat to compare
	 * @return true if left Chat should be displayed before right one
	 *
	 * This method compares two Chats instances. Chats are compared only by their names.
	 */
	bool lessThan(const Chat &left, const Chat &right) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Compares two Buddy instances.
	 * @param left Buddy to compare
	 * @param right Buddy to compare
	 * @return true if left Buddy should be displayed before right one
	 *
	 * This method compares two Buddy instances:
	 * <ul>
	 *   <li>Buddy with contacts is displayed before Buddy without contacts</li>
	 *   <li>non blocked Buddy is displayed before blocked Buddy</li>
	 *   <li>Buddy with unread messages is displayed before Buddy without unread messages</li>
	 *   <li>if sorting by status is enabled then statuses of preffered contacts of these Buddy instances are compared</li>
	 *   <li>at last, buddies are compared by their display() values</li>
	 * </ul>
	 */
	bool lessThan(const Buddy &left, const Buddy &right) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Decide if given Chat object should be displayed.
	 * @param chat Chat object to make decision of
	 *
	 * If no filters are installed then every Chat object is automatically accepted. If filters are installed
	 * then every one of them is asked for decision (using acceptChat() method). If any filter responds with
	 * Accepted then given chat is accepted and no further filters are asked. If any filter reponds with Rejected
	 * then given chat is rejected and no further filters are asked. If given filter respones with Undecided,
	 * then next one is asked for decision.
	 */
	bool accept(const Chat &chat) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Decide if given Buddy object should be displayed.
	 * @param chat Buddy object to make decision of
	 *
	 * If no filters are installed then every Buddy object is automatically accepted. If filters are installed
	 * then every one of them is asked for decision (using acceptBuddy() method). If any filter responds with
	 * Accepted then given chat is accepted and no further filters are asked. If any filter reponds with Rejected
	 * then given chat is rejected and no further filters are asked. If given filter respones with Undecided,
	 * then next one is asked for decision.
	 */
	bool accept(const Buddy &buddy) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Decide if given Contact object should be displayed.
	 * @param chat Contact object to make decision of
	 *
	 * If no filters are installed then every Contact object is automatically accepted. If filters are installed
	 * then every one of them is asked for decision (using acceptContact() method). If any filter responds with
	 * Accepted then given chat is accepted and no further filters are asked. If any filter reponds with Rejected
	 * then given chat is rejected and no further filters are asked. If given filter respones with Undecided,
	 * then next one is asked for decision.
	 */
	bool accept(const Contact &contact) const;

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Compares two indexes.
	 * @param left QModelIndex to compare
	 * @param right QModelIndex to compare
	 * @return true if left index contains data that should be displayed before right one
	 *
	 * This method compares two QModelIndex instances. If these indexes contains objects of different types,
	 * then Chat is displayed first, then Buddy, then Contact. If these indexes contains objects of the same
	 * types, then lessThan(Chat,Chat) method is used for Chat object and lessThan(Buddy,Buddy) method is
	 * used to compare two Buddy object and Contact objects (using owner buddy of these Contact objects).
	 */
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Decide if given item should be displayed.
	 * @param sourceRow row of item in source model
	 * @param sourceParent parent of item in source model
	 * @return true if given item should be displayed
	 *
	 * This method received object from source model and checks its type. For every supported type - Chat,
	 * Buddy and Contact suitable filter() method is called to get answer for this method. For any other
	 * type assertion is thrown.
	 */
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of TalkableProxyModel with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit TalkableProxyModel(QObject *parent = 0);
	virtual ~TalkableProxyModel();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Adds new filter to this object.
	 * @param filter new filter to add
	 *
	 * This method adds new filter to this object. Filter will be only added if it not already present.
	 * Current model is invalidated after adding new filter.
	 */
	void addFilter(TalkableFilter *filter);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Remove filter from this object.
	 * @param filter filter to remove
	 *
	 * This method removes filter from this object. Filter will not be removed if not present.
	 * Current model is invalidated after removing filter.
	 */
	void removeFilter(TalkableFilter *filter);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Enable or disable sorting by status and unread messages.
	 * @param sortByStatusAndUnreadMessages if true, sorting by status and unread messages will be enabled
	 *
	 * This method enables or disables sorting by status and unread messages. If method of sorting is changed then
	 * current model will be invalidated.
	 *
	 * Default value is true.
	 */
	void setSortByStatusAndUnreadMessages(bool sortByStatusAndUnreadMessages);

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited after filtering or sorting was changed.
	 */
	void invalidated();

};

/**
 * @}
 */

#endif // TALKABLE_PROXY_MODEL_H
