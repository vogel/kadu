/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef TYPING_NOTIFICATION_SERVICE_H
#define TYPING_NOTIFICATION_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

class Chat;
class Contact;

class KADUAPI TypingNotificationService : public QObject
{
	Q_OBJECT

public:
	TypingNotificationService(QObject *parent = 0) : QObject(parent) {}

public slots:
	virtual bool typingStarted(const Chat &chat) = 0;
	virtual bool typingStopped(const Chat &chat) = 0;

signals:
	void contactStartedTyping(const Contact &contact);
	void contactStoppedTyping(const Contact &contact);

};

#endif // TYPING_NOTIFICATION_SERVICE_H
