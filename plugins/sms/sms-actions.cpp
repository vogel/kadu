/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/kadu-window.h"
#include "debug.h"

#include "gui/windows/sms-dialog.h"
#include "gui/windows/sms-image-dialog.h"

#include "sms-actions.h"

SmsActions *SmsActions::Instance = 0;

void SmsActions::registerActions(bool firstLoad)
{
	if (Instance)
		return;

	Instance = new SmsActions();

	Q_UNUSED(firstLoad);
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
	connect(Core::instance()->kaduWindow(), SIGNAL(talkableActivated(Talkable)),
			this, SLOT(talkableActivated(Talkable)));

	sendSmsActionDescription = new ActionDescription(this,
		ActionDescription::TypeGlobal, "sendSmsAction",
		this, SLOT(sendSmsActionActivated(QAction *)),
		KaduIcon("phone"), tr("Send SMS...")
	);
	sendSmsActionDescription->setShortcut("kadu_sendsms");
	TalkableMenuManager::instance()->addActionDescription(sendSmsActionDescription, TalkableMenuItem::CategoryActions, 100);
	Core::instance()->kaduWindow()->insertMenuActionDescription(sendSmsActionDescription, KaduWindow::MenuBuddies, 5);
}

SmsActions::~SmsActions()
{
	disconnect(Core::instance()->kaduWindow(), SIGNAL(talkableActivated(Talkable)),
			this, SLOT(talkableActivated(Talkable)));

	TalkableMenuManager::instance()->removeActionDescription(sendSmsActionDescription);
	Core::instance()->kaduWindow()->removeMenuActionDescription(sendSmsActionDescription);
}

void SmsActions::newSms(const QString &mobile)
{
	SmsDialog *smsDialog = new SmsDialog();
	smsDialog->setRecipient(mobile);
	smsDialog->show();
}

void SmsActions::talkableActivated(const Talkable &talkable)
{
	const Buddy &buddy = talkable.toBuddy();
	if (buddy.contacts().isEmpty() && !buddy.mobile().isEmpty())
		newSms(buddy.mobile());
}

void SmsActions::sendSmsActionActivated(QAction *sender)
{
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	newSms(action->context()->buddies().toBuddy().mobile());
}
