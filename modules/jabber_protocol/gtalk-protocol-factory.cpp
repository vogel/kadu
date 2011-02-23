/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/misc.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"
#include "icons-manager.h"

#include "gui/widgets/jabber-contact-personal-info-widget.h"
#include "gui/widgets/jabber-add-account-widget.h"
#include "gui/widgets/jabber-create-account-widget.h"
#include "gui/widgets/jabber-edit-account-widget.h"
#include "jabber-account-details.h"
#include "jabber-contact-details.h"
#include "jabber-protocol.h"

#include "gtalk-protocol-factory.h"

GTalkProtocolFactory * GTalkProtocolFactory::Instance = 0;

void GTalkProtocolFactory::createInstance()
{
	if (!Instance)
		Instance = new GTalkProtocolFactory();
}

void GTalkProtocolFactory::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

GTalkProtocolFactory::GTalkProtocolFactory()
{
}

QIcon GTalkProtocolFactory::icon()
{
	return IconsManager::instance()->iconByPath("protocols/xmpp/brand_name/GmailGoogleTalk");
}

QString GTalkProtocolFactory::iconPath()
{
	return IconsManager::instance()->iconPath("protocols/xmpp/brand_name/GmailGoogleTalk", "16x16");
}

QString GTalkProtocolFactory::idLabel()
{
	return tr("Gmail/Google Talk ID:");
}

QString GTalkProtocolFactory::defaultServer()
{
    return QLatin1String("gmail.com");
}
