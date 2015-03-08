/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-register-account-service.h"

#include "services/jabber-error-service.h"
#include "services/jabber-register-account.h"

#include <qxmpp/QXmppClient.h>

JabberRegisterAccountService::JabberRegisterAccountService(QObject *parent) :
		QObject{parent}
{
}

JabberRegisterAccountService::~JabberRegisterAccountService()
{
}

void JabberRegisterAccountService::setErrorService(JabberErrorService *errorService)
{
	m_errorService = errorService;
}

JabberRegisterAccount * JabberRegisterAccountService::registerAccount(Jid jid, QString password, QString email)
{
	auto result = new JabberRegisterAccount{std::move(jid), std::move(password), std::move(email), this};
	result->setErrorService(m_errorService);
	return result;
}

#include "moc_jabber-register-account-service.cpp"
