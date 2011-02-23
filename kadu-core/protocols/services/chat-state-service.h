/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef CHAT_STATE_SERVICE_H
#define CHAT_STATE_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

class Chat;
class Contact;

class KADUAPI ChatStateService : public QObject
{
	Q_OBJECT

public:
	enum ContactActivity
	{
		StateActive,
		StateComposing,
		StateGone,
		StateInactive,
		StateNone,
		StatePaused
	};

	ChatStateService(QObject *parent = 0) : QObject(parent) {}

public:
	virtual void composingStarted(const Chat &chat) = 0;
	virtual void composingStopped(const Chat &chat) = 0;

	virtual void chatWidgetClosed(const Chat &chat) = 0;
	virtual void chatWidgetActivated(const Chat &chat) = 0;
	virtual void chatWidgetDeactivated(const Chat &chat) = 0;

signals:
	void contactActivityChanged(ChatStateService::ContactActivity state, const Contact &contact);
};

#endif // CHAT_STATE_SERVICE_H
