/*
 * %kadu copyright begin%
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

#ifndef ENCRYPTION_ACTIONS_H
#define ENCRYPTION_ACTIONS_H

#include <QtCore/QObject>
#include <QtGui/QAction>

#include "chat/chat.h"

class ActionDescription;

class EncryptionActions : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EncryptionActions)

	static EncryptionActions *Instance;

	ActionDescription *EnableEncryptionActionDescription;
	ActionDescription *SendPublicKeyActionDescription;

	EncryptionActions(bool firstLoad);
	virtual ~EncryptionActions();

	void sendPublicKey(const Contact &contact);

private slots:
	void canEncryptChanged(const Chat &chat);

	void enableEncryptionActionActivated(QAction *sender, bool toggled);
	void sendPublicKeyActionActivated(QAction *sender, bool toggled);

public:
	static void registerActions(bool firstLoad);
	static void unregisterActions();

	static EncryptionActions * instance() { return Instance; }

	void checkEnableEncryption(const Chat &chat, bool check);

};

#endif // ENCRYPTION_ACTIONS_H
