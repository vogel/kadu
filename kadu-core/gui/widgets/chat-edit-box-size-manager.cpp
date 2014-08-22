/*
 * %kadu copyright begin%
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"

#include "chat-edit-box-size-manager.h"

ChatEditBoxSizeManager * ChatEditBoxSizeManager::Instance = 0;

ChatEditBoxSizeManager * ChatEditBoxSizeManager::instance()
{
	if (!Instance)
		Instance = new ChatEditBoxSizeManager();

	return Instance;
}

ChatEditBoxSizeManager::ChatEditBoxSizeManager() :
		CommonHeight(0)
{
	configurationUpdated();
}

ChatEditBoxSizeManager::~ChatEditBoxSizeManager()
{
}

void ChatEditBoxSizeManager::configurationUpdated()
{
	setCommonHeight(Application::instance()->configuration()->deprecatedApi()->readNumEntry("Chat", "ChatEditBoxHeight", 0));
}

void ChatEditBoxSizeManager::setCommonHeight(int height)
{
	if (height != CommonHeight)
	{
		CommonHeight = height;
		Application::instance()->configuration()->deprecatedApi()->writeEntry("Chat", "ChatEditBoxHeight", CommonHeight);
		emit commonHeightChanged(CommonHeight);
	}
}

bool ChatEditBoxSizeManager::initialized()
{
	return 0 != CommonHeight;
}

#include "moc_chat-edit-box-size-manager.cpp"
