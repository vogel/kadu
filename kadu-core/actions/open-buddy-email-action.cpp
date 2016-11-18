/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "open-buddy-email-action.h"

#include "contacts/contact-set.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "os/generic/url-opener.h"
#include "url-handlers/url-handler-manager.h"

OpenBuddyEmailAction::OpenBuddyEmailAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"mail-message-new"});
	setName(QStringLiteral("writeEmailAction"));
	setText(tr("Send E-Mail"));
	setType(ActionDescription::TypeUser);
}

OpenBuddyEmailAction::~OpenBuddyEmailAction()
{
}

void OpenBuddyEmailAction::setUrlHandlerManager(UrlHandlerManager *urlHandlerManager)
{
	m_urlHandlerManager = urlHandlerManager;
}

void OpenBuddyEmailAction::setUrlOpener(UrlOpener *urlOpener)
{
	m_urlOpener = urlOpener;
}

void OpenBuddyEmailAction::actionInstanceCreated(Action* action)
{
	auto const &buddy = action->context()->buddies().toBuddy();
	if (buddy)
		connect(buddy, SIGNAL(updated()), action, SLOT(checkState()));
}

void OpenBuddyEmailAction::actionTriggered(QAction *sender, bool)
{
	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	const Buddy &buddy = action->context()->buddies().toBuddy();
	if (!buddy)
		return;

	if (!buddy.email().isEmpty())
		m_urlOpener->openEmail(buddy.email().toUtf8());
}

void OpenBuddyEmailAction::updateActionState(Action* action)
{
	auto const &buddy = action->context()->buddies().toBuddy();
	auto hasMail = !buddy.email().isEmpty() && buddy.email().indexOf(m_urlHandlerManager->mailRegExp()) == 0;

	action->setEnabled(hasMail);
}

#include "moc_open-buddy-email-action.cpp"
