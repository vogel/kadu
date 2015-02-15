/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_ADAPTER_H
#define CHAT_ADAPTER_H

#include <QtCore/QObject>

#include "contacts/contact.h"

class Chat;
class ContactListModel;

/**
 * @addtogroup Contact
 * @{
 */

/**
 * @class ChatAdapter
 * @author Rafał 'Vogel' Malinowski
 * @short Adapter that makes ContactListModel to use copy of buddies from given Chat.
 *
 * This adapter created with a @link ContactListModel @endlink as parent replicates list of buddies from
 * @link Chat @endlink on this model.
 */
class ChatAdapter : public QObject
{
	Q_OBJECT

	ContactListModel *Model;

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called after a contact is added to @link Chat @endlink.
	 * @param contact added contact
	 *
	 * Contact is added to @link ContactListModel @endlink instance.
	 */
	void contactAdded(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called after a contact is removed from @link Chat @endlink.
	 * @param contact removed contact
	 *
	 * Contact is removed from @link ContactListModel @endlink instance.
	 */
	void contactRemoved(const Contact &contact);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new ChatAdapter on given @link ContactListModel @endlink for given @link Chat @endlink.
	 * @param model model to adapt @link Chat @endlink to
	 *
	 * Given @link ContactListModel @endlink will now have exactly the same list of contacts as given @link Chat @endlink.
	 */
	explicit ChatAdapter(ContactListModel *model, const Chat &chat);
	virtual ~ChatAdapter();

};

/**
 * @}
 */

#endif // CHAT_ADAPTER_H
