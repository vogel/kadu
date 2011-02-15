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

#ifndef COMPOSING_NOTIFICATION_SERVICE_H
#define COMPOSING_NOTIFICATION_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

class Chat;
class Contact;

class KADUAPI ComposingNotificationService : public QObject
{
	Q_OBJECT

public:
	ComposingNotificationService(QObject *parent = 0) : QObject(parent) {}

public slots:
	virtual bool composingStarted(const Chat &chat) = 0;
	virtual bool composingStopped(const Chat &chat) = 0;

signals:
	void contactStartedComposing(const Contact &contact);
	void contactStoppedComposing(const Contact &contact);

};

#endif // COMPOSING_NOTIFICATION_SERVICE_H
