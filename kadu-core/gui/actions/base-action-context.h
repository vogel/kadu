/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2012 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BASE_ACTION_CONTEXT_H
#define BASE_ACTION_CONTEXT_H

#include <QtCore/QObject>

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "model/roles.h"

#include "gui/actions/action-context.h"

#include "exports.h"

class KADUAPI BaseActionContext : public ActionContext
{
	Q_OBJECT

	ChangeNotifier MyChangeNotifier;

	QWidget *Widget;
	ContactSet Contacts;
	BuddySet Buddies;
	Chat CurrentChat;
	StatusContainer *CurrentStatusContainer;
	RoleSet Roles;

public:
	explicit BaseActionContext(QWidget *widget);
	virtual ~BaseActionContext();

	ChangeNotifier & changeNotifier();

	virtual QWidget * widget() override;

	virtual ContactSet contacts() override;
	void setContacts(const ContactSet &contacts);

	virtual BuddySet buddies() override;
	void setBuddies(const BuddySet &buddies);

	virtual Chat chat() override;
	void setChat(const Chat &chat);

	virtual StatusContainer * statusContainer() override;
	void setStatusContainer(StatusContainer *statusContainer);

	virtual RoleSet roles() override;
	void setRoles(const RoleSet &roles);

};

#endif // BASE_ACTION_CONTEXT_H
