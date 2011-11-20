/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef OPEN_CHAT_WITH_H
#define OPEN_CHAT_WITH_H

#include <QtGui/QWidget>

#include "os/generic/desktop-aware-object.h"

#include "open-chat-with-runner.h"

class QLabel;
class QPushButton;
class QVBoxLayout;

class BuddyListModel;
class LineEditWithClearButton;
class ModelChain;
class TalkableTreeView;

class KADUAPI OpenChatWith : public QWidget, DesktopAwareObject
{
	Q_OBJECT

	static OpenChatWith *Instance;

	explicit OpenChatWith();

	bool IsTyping;

	TalkableTreeView *BuddiesWidget;
	LineEditWithClearButton *ContactID;
	QVBoxLayout *MainLayout;
	OpenChatWithRunner *OpenChatRunner;

	BuddyListModel *ListModel;
	ModelChain *Chain;

private slots:
	void inputAccepted();
	void inputChanged(const QString &text);
	void openChat();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);

public:
  	static OpenChatWith * instance();

	virtual ~OpenChatWith();

	void show();
};

#endif // OPEN_CHAT_WITH_H
