/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "os/generic/desktop-aware-object.h"
#include "open-chat-with-runner.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class BuddyListModel;
class BuddyManager;
class ChatManager;
class ChatWidgetManager;
class InjectedFactory;
class LineEditWithClearButton;
class ModelChain;
class PathsProvider;

class QLabel;
class QPushButton;
class QQuickWidget;
class QVBoxLayout;

class KADUAPI OpenChatWith : public QWidget, DesktopAwareObject
{
	Q_OBJECT

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	explicit OpenChatWith(QWidget *parent = nullptr);
	virtual ~OpenChatWith();

	void show();

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<PathsProvider> m_pathsProvider;

	QQuickWidget *BuddiesView;
	LineEditWithClearButton *ContactID;
	QVBoxLayout *MainLayout;
	OpenChatWithRunner *OpenChatRunner;

	BuddyListModel *ListModel;
	ModelChain *Chain;

	void focusQml();

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

	void inputAccepted();
	void inputChanged(const QString &text);
	void itemActivated(int index);

};
