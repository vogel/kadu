/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QTabWidget;

class Chat;
class ChatHistoryTab;
class Configuration;
class HistoryMessagesTab;
class HistoryStorage;
class History;
class IconsManager;
class PluginInjectedFactory;
class SearchTab;
class SessionService;

/*!
\class HistoryWindow
\author Juzef, Vogel
*/
class HistoryWindow : public QWidget
{
	Q_OBJECT

	friend class History;

	QPointer<Configuration> m_configuration;
	QPointer<History> m_history;
	QPointer<IconsManager> m_iconsManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<SessionService> m_sessionService;

	QTabWidget *TabWidget;
	int CurrentTab;

	ChatHistoryTab *ChatTab;
	HistoryMessagesTab *StatusTab;
	HistoryMessagesTab *SmsTab;
	SearchTab *MySearchTab;

	void createGui();

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setHistory(History *history);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setSessionService(SessionService *sessionService);
	INJEQT_INIT void init();

	void currentTabChanged(int newTabIndex);
	void storageChanged(HistoryStorage *historyStorage);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	explicit HistoryWindow(QWidget *parent = nullptr);
	virtual ~HistoryWindow();

	void updateData();
	void selectChat(const Chat &chat);

};
