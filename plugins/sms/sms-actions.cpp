/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

#include "actions/action-description.h"
#include "actions/action.h"
#include "contacts/contact-set.h"
#include "menu/menu-inventory.h"
#include "plugin/plugin-injected-factory.h"
#include "talkable/talkable-converter.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/talkable-tree-view.h"
#include "windows/kadu-window-service.h"
#include "windows/kadu-window.h"
#include "debug.h"

#include "plugins/history/history.h"

#include "gui/windows/sms-dialog-repository.h"
#include "gui/windows/sms-dialog.h"
#include "scripts/sms-script-manager.h"
#include "mobile-number-manager.h"
#include "send-sms-action.h"
#include "sms-gateway-manager.h"

#include "sms-actions.h"

SmsActions::SmsActions(QObject *parent) :
		QObject{parent}
{
}

SmsActions::~SmsActions()
{
}

void SmsActions::setHistory(History *history)
{
	m_history = history;
}

void SmsActions::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void SmsActions::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void SmsActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void SmsActions::setMobileNumberManager(MobileNumberManager *mobileNumberManager)
{
	m_mobileNumberManager = mobileNumberManager;
}

void SmsActions::setSendSmsAction(SendSmsAction *sendSmsAction)
{
	m_sendSmsAction = sendSmsAction;
}

void SmsActions::setSmsDialogRepository(SmsDialogRepository *smsDialogRepository)
{
	m_smsDialogRepository = smsDialogRepository;
}

void SmsActions::setSmsGatewayManager(SmsGatewayManager *smsGatewayManager)
{
	m_smsGatewayManager = smsGatewayManager;
}

void SmsActions::setSmsScriptsManager(SmsScriptsManager *smsScriptsManager)
{
	m_smsScriptsManager = smsScriptsManager;
}

void SmsActions::setTalkableConverter(TalkableConverter *talkableConverter)
{
	m_talkableConverter = talkableConverter;
}

void SmsActions::init()
{
	connect(m_kaduWindowService->kaduWindow(), SIGNAL(talkableActivated(Talkable)),
			this, SLOT(talkableActivated(Talkable)));

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_sendSmsAction, KaduMenu::SectionSend, 10)
		->update();
	m_menuInventory
		->menu("buddy")
		->addAction(m_sendSmsAction, KaduMenu::SectionBuddies, 5)
		->update();
}

void SmsActions::done()
{
	disconnect(m_kaduWindowService->kaduWindow(), 0, this, 0);

	m_menuInventory
		->menu("buddy-list")
		->removeAction(m_sendSmsAction)
		->update();
	m_menuInventory
		->menu("buddy")
		->removeAction(m_sendSmsAction)
		->update();
}

void SmsActions::newSms(const QString &mobile)
{
	auto smsDialog = m_pluginInjectedFactory->makeInjected<SmsDialog>(m_history, m_mobileNumberManager, m_smsGatewayManager, m_smsScriptsManager);
	if (m_smsDialogRepository)
		m_smsDialogRepository->addDialog(smsDialog);

	smsDialog->setRecipient(mobile);
	smsDialog->show();
}

void SmsActions::talkableActivated(const Talkable &talkable)
{
	const Buddy &buddy = m_talkableConverter->toBuddy(talkable);
	if (buddy.contacts().isEmpty() && !buddy.mobile().isEmpty())
		newSms(buddy.mobile());
}

#include "moc_sms-actions.cpp"
