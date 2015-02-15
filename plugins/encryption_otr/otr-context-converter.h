/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

extern "C" {
#	include <libotr/context.h>
}

#include "storage/manager-common.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Chat;
class Contact;

class OtrUserStateService;

class OtrContextConverter : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE OtrContextConverter();
	virtual ~OtrContextConverter();

	Chat connectionContextToChat(ConnContext *context) const;
	Contact connectionContextToContact(ConnContext *context) const;

	ConnContext * chatToContextConverter(const Chat &chat, NotFoundAction notFoundAction = ActionReturnNull) const;
	ConnContext * contactToContextConverter(const Contact &contact, NotFoundAction notFoundAction = ActionReturnNull) const;

private slots:
	INJEQT_SETTER void setUserStateService(OtrUserStateService *userStateService);

private:
	QPointer<OtrUserStateService> UserStateService;

};
