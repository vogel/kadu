/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtGui/QIcon>

class Chat;
class ChatWidget;

enum class ChatWidgetTitleComposingStatePosition;

class KADUAPI ChatWidgetSetTitle : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetSetTitle(QObject *parent = nullptr);
	virtual ~ChatWidgetSetTitle();

	void setDefaultTile(QString defaultTitle);
	void setDefaultIcon(QIcon icon);
	void setActiveChatWidget(ChatWidget *chatWidget);

	QString fullTitle() const;
	QIcon icon() const;

public slots:
	void addChatWidget(ChatWidget *chatWidget);
	void removeChatWidget(ChatWidget *chatWidget);

signals:
	void titleChanged();

private:
	QList<ChatWidget *> m_chatWidgets;
	QPointer<ChatWidget> m_currentChatWidget;
	QPointer<ChatWidget> m_activeChatWidget;
	QPointer<ChatWidget> m_unreadMessagesChatWidget;

	QString m_defaultTitle;
	QIcon m_defaultIcon;

	void selectNewUnreadMessagesChatWidget();
	void setUnreadMessagesChatWidget(ChatWidget *chatWidget);
	void setCurrentChatWidget(ChatWidget *chatWidget);

private slots:
	void chatUpdated();
	void chatWidgetDestroyed(ChatWidget *chatWidget);
	void chatWidgetDestroyed(Chat chat);

	void update();

};
