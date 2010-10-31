/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "facebook-protocol-factory.h"

FacebookProtocolFactory * FacebookProtocolFactory::Instance = 0;

FacebookProtocolFactory * FacebookProtocolFactory::instance()
{
	if (!Instance)
		Instance = new FacebookProtocolFactory();

	return Instance;
}

FacebookProtocolFactory::FacebookProtocolFactory()
{
}

Protocol * FacebookProtocolFactory::createProtocolHandler(Account account)
{
	JabberProtocol *result = static_cast<JabberProtocol *>(JabberProtocolFactory::createProtocolHandler(account));
	result->setContactsListReadOnly(true);

	return result;
}

QIcon FacebookProtocolFactory::icon()
{
	return IconsManager::instance()->iconByPath("protocols/xmpp/brand_name/facebook");
}

QString FacebookProtocolFactory::iconPath()
{
	return IconsManager::instance()->iconPath("protocols/xmpp/brand_name/facebook", "16x16");
}

QString FacebookProtocolFactory::idLabel()
{
	return tr("Facebook ID:");
}

bool FacebookProtocolFactory::allowChangeServer()
{
    return false;
}

QString FacebookProtocolFactory::defaultServer()
{
    return QLatin1String("chat.facebook.com");
}
