/*
 * %kadu copyright begin%
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CHAT_DATA_WINDOW_H
#define CHAT_DATA_WINDOW_H

#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "chat/chat.h"

#include "exports.h"

class QLineEdit;
class QPushButton;

class GroupList;

class KADUAPI ChatDataWindow : public QWidget
{
	Q_OBJECT

	static QMap<Chat, ChatDataWindow *> Instances;

	Chat MyChat;

	QLineEdit *DisplayEdit;
	GroupList *ChatGroupList;

	QPushButton *OkButton;
	QPushButton *ApplyButton;

	ChatDataWindow(const Chat &chat, QWidget *parent);
	virtual ~ChatDataWindow();

	void createGui();
	void createButtons(QLayout *layout);

	bool isValid();

private slots:
	void updateButtons();
	void updateChat();
	void updateChatAndClose();
	void chatRemoved(const Chat &buddy);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	static ChatDataWindow * instance(const Chat &chat, QWidget *parent = 0);

	void show();

	Chat chat() const { return MyChat; }

};

#endif // CHAT_DATA_WINDOW_H
