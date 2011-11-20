/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"

#include "gui/widgets/jabber-add-account-widget.h"
#include "gui/widgets/jabber-contact-personal-info-widget.h"
#include "gui/widgets/jabber-create-account-widget.h"
#include "gui/widgets/jabber-edit-account-widget.h"
#include "jabber-account-details.h"
#include "jabber-contact-details.h"
#include "jabber-protocol.h"

#include "facebook-protocol-factory.h"

FacebookProtocolFactory * FacebookProtocolFactory::Instance = 0;

void FacebookProtocolFactory::createInstance()
{
	if (!Instance)
		Instance = new FacebookProtocolFactory();
}

void FacebookProtocolFactory::destroyInstance()
{
	delete Instance;
	Instance = 0;
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

KaduIcon FacebookProtocolFactory::icon()
{
	return KaduIcon("protocols/xmpp/brand_name/facebook", "16x16");
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

QString FacebookProtocolFactory::whatIsMyUsername()
{
	return tr(
		"Your username is available at <a href='https://www.facebook.com/editaccount.php?settings'>https://www.facebook.com/editaccount.php?settings</a> under Username field. "
		"If this field is empty, you can choose your Username and enter it there."
	);
}
