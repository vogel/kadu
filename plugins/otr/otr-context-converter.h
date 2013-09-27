/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_CONTEXT_CONVERTER_H
#define OTR_CONTEXT_CONVERTER_H

extern "C" {
#	include <libotr/context.h>
}

#include <QtCore/QObject>

#include "storage/manager-common.h"

class Chat;
class Contact;

class OtrUserStateService;

class OtrContextConverter : public QObject
{
	Q_OBJECT

	QWeakPointer<OtrUserStateService> UserStateService;

public:
	explicit OtrContextConverter(QObject *parent = 0);
	virtual ~OtrContextConverter();

	void setUserStateService(OtrUserStateService *userStateService);

	Chat connectionContextToChat(ConnContext *context) const;
	Contact connectionContextToContact(ConnContext *context) const;

	ConnContext * chatToContextConverter(const Chat &chat, NotFoundAction notFoundAction = ActionReturnNull) const;
	ConnContext * contactToContextConverter(const Contact &contact, NotFoundAction notFoundAction = ActionReturnNull) const;

};

#endif // OTR_CONTEXT_CONVERTER_H
