/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QMenu>

#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/windows/kadu-window.h"
#include "debug.h"

#include "gui/windows/sms-image-dialog.h"
#include "gui/windows/sms-dialog.h"
#include "sms.h"

#include "sms-actions.h"

SmsActions *SmsActions::Instance = 0;

void SmsActions::registerActions(bool firstLoad)
{
	Instance = new SmsActions();

	Q_UNUSED(firstLoad);
// 	if (firstLoad)
// 		ChatEditBox::addAction("ScreenShotAction");
}

void SmsActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

SmsActions * SmsActions::instance()
{
	return Instance;
}

SmsActions::SmsActions()
{	
	//TODO 0.6.6
  	//QObject::connect(Core::instance()->kaduWindow()->contactsListView(), SIGNAL(chatActivated(Chat *)), this, SLOT(onUserDblClicked(Chat *)));
	
	sendSmsActionDescription = new ActionDescription(this,
		ActionDescription::TypeGlobal, "sendSmsAction",
		this, SLOT(sendSmsActionActivated(QAction *, bool)),
		"16x16/phone.png", "16x16/phone.png", tr("Send SMS..."), false
	);
	sendSmsActionDescription->setShortcut("kadu_sendsms");
	BuddiesListViewMenuManager::instance()->insertActionDescription(4, sendSmsActionDescription);
	Core::instance()->kaduWindow()->insertMenuActionDescription(sendSmsActionDescription, KaduWindow::MenuContacts, 4);
}

SmsActions::~SmsActions()
{
  	//TODO 0.6.6
	//QObject::disconnect(Core::instance()->kaduWindow()->contactsListView(), SIGNAL(chatActivated(Chat *)), this, SLOT(onUserDblClicked(Chat *)));
	
	BuddiesListViewMenuManager::instance()->removeActionDescription(sendSmsActionDescription);
	Core::instance()->kaduWindow()->removeMenuActionDescription(sendSmsActionDescription);
	delete sendSmsActionDescription;
	sendSmsActionDescription = 0;
}

void SmsActions::newSms(QString nick)
{
	SmsDialog *smsDialog = new SmsDialog();
	smsDialog->setRecipient(nick);
	smsDialog->show();
}

void SmsActions::onUserDblClicked(Chat *chat)
{
	kdebugf();
	Buddy buddy = chat->contacts().toContactList().at(0).ownerBuddy();
	if (buddy.contacts().isEmpty() && !buddy.mobile().isEmpty())
		newSms(buddy.mobile());
	kdebugf2();
}

void SmsActions::sendSmsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (action)
	{
		BuddySet users = action->buddies();

		if (users.count() == 1 && !users.toList()[0].mobile().isEmpty())
		{
			newSms(users.toList()[0].mobile());
			return;
		}
	}
	newSms(QString::null);

	kdebugf2();
}
