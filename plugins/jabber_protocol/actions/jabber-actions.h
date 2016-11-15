/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QAction;
class QMenu;

class ActionDescription;
class Actions;
class Action;
class Contact;
class PluginInjectedFactory;
class Myself;
class ShowXmlConsoleAction;
class SubscriptionService;

// TODO: this class can be moved to core and just check for SubscriptionService
class JabberActions : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit JabberActions(QObject *parent = nullptr);
	virtual ~JabberActions();

	ActionDescription * resendSubscription() { return ResendSubscription; }
	ActionDescription * removeSubscription() { return RemoveSubscription; }
	ActionDescription * askForSubscription() { return AskForSubscription; }

private:
	QPointer<Actions> m_actions;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<Myself> m_myself;
	QPointer<ShowXmlConsoleAction> m_showXmlConsoleAction;

	ActionDescription *ResendSubscription;
	ActionDescription *RemoveSubscription;
	ActionDescription *AskForSubscription;

	Contact contactFromAction(QAction *action);
	SubscriptionService * subscriptionServiceFromContact(const Contact &contact);

private slots:
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setMyself(Myself *myself);
	INJEQT_SET void setShowXmlConsoleAction(ShowXmlConsoleAction *showXmlConsoleAction);
	INJEQT_INIT void init();

	void resendSubscriptionActionActivated(QAction *sender);
	void removeSubscriptionActionActivated(QAction *sender);
	void askForSubscriptionActionActivated(QAction *sender);

};
