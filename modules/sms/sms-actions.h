/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_ACTIONS_H
#define SMS_ACTIONS_H

#include <QtCore/QObject>

#include "buddies/buddy.h"
#include "chat/chat.h"

class QAction;

class ActionDescription;
class ChatWidget;

class SmsActions : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SmsActions)

	static SmsActions *Instance;

	ActionDescription *sendSmsActionDescription;

	SmsActions();
	virtual ~SmsActions();

private slots:
	void buddyActivated(Buddy buddy);
	void sendSmsActionActivated(QAction *sender);
	void newSms(const QString &mobile);

public:
	static void registerActions(bool firstLoad);
	static void unregisterActions();

	static SmsActions * instance();

};

#endif // SMS_ACTIONS_H
