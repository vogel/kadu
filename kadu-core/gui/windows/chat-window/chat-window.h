/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
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

#include <QtGui/QWidget>

#include "configuration/configuration-aware-object.h"
#include "gui/widgets/chat-widget/chat-widget-container.h"
#include "os/generic/compositing-aware-object.h"
#include "os/generic/desktop-aware-object.h"
#include "exports.h"

class QTimer;

class Chat;
class ChatWidget;

class KADUAPI ChatWindow : public QWidget, public ChatWidgetContainer, ConfigurationAwareObject, CompositingAwareObject, DesktopAwareObject
{
	Q_OBJECT

public:
	explicit ChatWindow(ChatWidget *chatWidget, QWidget *parent = 0);
	virtual ~ChatWindow();

	Chat chat() const;
	ChatWidget * chatWidget() const { return m_chatWidget; }

	virtual void tryActivateChatWidget(ChatWidget *chatWidget) override;
	virtual void alertChatWidget(ChatWidget *chatWidget) override;
	virtual void closeChatWidget(ChatWidget *chatWidget) override;
	virtual bool isChatWidgetActive(const ChatWidget *chatWidget) override;

	void setWindowTitle(QString title);

public slots:
	// TODO: rename
	void blinkTitle();
	void showNewMessagesNumInTitle();

signals:
	void windowDestroyed(ChatWindow *chatWindow);
	void windowDestroyed(ChatWidget * const chatWidget);

protected:
	virtual void closeEvent(QCloseEvent *e) override;
	virtual void changeEvent(QEvent *event) override;

	virtual void configurationUpdated() override;

	virtual void compositingEnabled() override;
	virtual void compositingDisabled() override;

private:
	ChatWidget *m_chatWidget;
	QTimer *m_titleTimer;

	bool m_showNewMessagesNum;
	bool m_blinkChatTitle;

	QRect defaultGeometry() const;

private slots:
	void updateIcon();
	void updateTitle();

};
