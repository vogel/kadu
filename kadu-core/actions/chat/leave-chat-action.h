/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "actions/action-description.h"
#include "injeqt-type-roles.h"

#include <injeqt/injeqt.h>

class ChatServiceRepository;
class ChatWidgetRepository;
class IconsManager;

class LeaveChatAction : public ActionDescription
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(ACTION)

public:
	Q_INVOKABLE explicit LeaveChatAction(QObject *parent = nullptr);
	virtual ~LeaveChatAction();

protected:
	virtual void triggered(QWidget *widget, ActionContext *context, bool toggled);

private:
	QPointer<ChatServiceRepository> m_chatServiceRepository;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<IconsManager> m_iconsManager;

private slots:
	INJEQT_SET void setChatServiceRepository(ChatServiceRepository *chatServiceRepository);
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);

};
