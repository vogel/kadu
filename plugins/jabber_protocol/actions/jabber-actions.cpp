/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/xml-console.h"
#include "protocols/protocol.h"

#include "services/jabber-subscription-service.h"
#include "jabber-protocol.h"

#include "jabber-actions.h"

static void disableNoRosterContact(Action *action)
{
	action->setEnabled(false);

	Contact contact = action->contact();
	if (!contact)
		return;

	if (action->buddies().contains(Core::instance()->myself()))
		return;

	Account account = contact.contactAccount();
	if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
		return;

	if (!account.protocolHandler()->isConnected())
		return;

	action->setEnabled(true);
}

JabberActions * JabberActions::Instance = 0;

void JabberActions::registerActions()
{
	if (!Instance)
		Instance = new JabberActions();
}

void JabberActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

JabberActions::JabberActions()
{
	ShowXmlConsole = new ActionDescription(this, ActionDescription::TypeMainMenu, "showXmlConsole",
			0, 0, KaduIcon(), tr("Show XML Console for Account"));
	connect(ShowXmlConsole, SIGNAL(actionCreated(Action*)), this, SLOT(showXmlConsoleActionCreated(Action*)));

	// HACK: It is needed bacause of loading protocol modules before creating GUI.
	// TODO 0.10: Fix it!
	QMetaObject::invokeMethod(this, "insertMenuToMainWindow", Qt::QueuedConnection);

	ShowXmlConsoleMenu = new QMenu();
	updateShowXmlConsoleMenu();
	connect(ShowXmlConsoleMenu, SIGNAL(triggered(QAction*)),
			this, SLOT(showXmlConsoleActionActivated(QAction*)));
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
			this, SLOT(updateShowXmlConsoleMenu()));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
			this, SLOT(updateShowXmlConsoleMenu()));

	ResendSubscription = new ActionDescription(this, ActionDescription::TypeUser, "rosterResendSubscription",
			this, SLOT(resendSubscriptionActionActivated(QAction*)), KaduIcon(), tr("Resend Subscription"),
			false, disableNoRosterContact);
	RemoveSubscription = new ActionDescription(this, ActionDescription::TypeUser, "rosterRemoveSubscription",
			this, SLOT(removeSubscriptionActionActivated(QAction*)), KaduIcon(), tr("Remove Subscription"),
			false, disableNoRosterContact);
	AskForSubscription = new ActionDescription(this, ActionDescription::TypeUser, "rosterAskForSubscription",
			this, SLOT(askForSubscriptionActionActivated(QAction*)), KaduIcon(), tr("Ask for Subscription"),
			false, disableNoRosterContact);
}

JabberActions::~JabberActions()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(ShowXmlConsole);

	delete ShowXmlConsoleMenu;
}

Contact JabberActions::contactFromAction(QAction *action)
{
	Action *kaduAction = qobject_cast<Action *>(action);
	if (!kaduAction)
		return Contact::null;

	return kaduAction->contact();
}

JabberSubscriptionService * JabberActions::subscriptionServiceFromContact(const Contact& contact)
{
	JabberProtocol *jabberProtocolHandler = qobject_cast<JabberProtocol *>(contact.contactAccount().protocolHandler());
	if (!jabberProtocolHandler)
		return 0;

	return jabberProtocolHandler->subscriptionService();
}

void JabberActions::resendSubscriptionActionActivated(QAction *sender)
{
	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	JabberSubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->resendSubscription(contact);
}

void JabberActions::removeSubscriptionActionActivated(QAction *sender)
{
	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	JabberSubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->removeSubscription(contact);
}

void JabberActions::askForSubscriptionActionActivated(QAction *sender)
{
	Contact contact = contactFromAction(sender);
	if (!contact)
		return;

	JabberSubscriptionService *subscriptionService = subscriptionServiceFromContact(contact);
	if (!subscriptionService)
		return;

	subscriptionService->requestSubscription(contact);
}

void JabberActions::showXmlConsoleActionCreated(Action *action)
{
	action->setMenu(ShowXmlConsoleMenu);
	action->setVisible(!ShowXmlConsoleMenu->actions().isEmpty());
}

void JabberActions::showXmlConsoleActionActivated(QAction *sender)
{
	(new XmlConsole(sender->data().value<Account>()))->show();
}

void JabberActions::updateShowXmlConsoleMenu()
{
	ShowXmlConsoleMenu->clear();

	foreach (const Account &account, AccountManager::instance()->items())
		if (account.protocolName() == QLatin1String("jabber"))
		{
			QAction *action = new QAction(QString("%1 (%2)").arg(account.accountIdentity().name(), account.id()), ShowXmlConsoleMenu);
			action->setData(QVariant::fromValue(account));
			ShowXmlConsoleMenu->addAction(action);
		}

	bool enable = !ShowXmlConsoleMenu->actions().isEmpty();
	foreach (Action *action, ShowXmlConsole->actions())
		action->setVisible(enable);
}

void JabberActions::insertMenuToMainWindow()
{
	Core::instance()->kaduWindow()->insertMenuActionDescription(ShowXmlConsole, KaduWindow::MenuTools);
}

