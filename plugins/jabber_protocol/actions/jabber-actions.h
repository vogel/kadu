/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef JABBER_ACTIONS_H
#define JABBER_ACTIONS_H

#include <QtCore/QObject>

class QAction;
class QMenu;

class Action;
class ActionDescription;
class Contact;
class JabberSubscriptionService;

class JabberActions : QObject
{
	Q_OBJECT

	static JabberActions *Instance;

	ActionDescription *ResendSubscription;
	ActionDescription *RemoveSubscription;
	ActionDescription *AskForSubscription;

	Contact contactFromAction(QAction *action);
	JabberSubscriptionService * subscriptionServiceFromContact(const Contact &contact);

	explicit JabberActions();
	virtual ~JabberActions();

private slots:
	void resendSubscriptionActionActivated(QAction *sender);
	void removeSubscriptionActionActivated(QAction *sender);
	void askForSubscriptionActionActivated(QAction *sender);

public:
	static void registerActions();
	static void unregisterActions();

	static JabberActions * instance() { return Instance; }

	ActionDescription * resendSubscription() { return ResendSubscription; }
	ActionDescription * removeSubscription() { return RemoveSubscription; }
	ActionDescription * askForSubscription() { return AskForSubscription; }

};

#endif // JABBER_ACTIONS_H
