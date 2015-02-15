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

#ifndef HISTORY_WINDOW_H
#define HISTORY_WINDOW_H

#include <QtWidgets/QWidget>

class QTabWidget;

class Chat;
class ChatHistoryTab;
class HistoryMessagesTab;
class HistoryStorage;
class SearchTab;

/*!
\class HistoryWindow
\author Juzef, Vogel
*/
class HistoryWindow : public QWidget
{
	Q_OBJECT

	friend class History;

	static HistoryWindow *Instance;

	QTabWidget *TabWidget;
	int CurrentTab;

	ChatHistoryTab *ChatTab;
	HistoryMessagesTab *StatusTab;
	HistoryMessagesTab *SmsTab;
	SearchTab *MySearchTab;

	explicit HistoryWindow(QWidget *parent = 0);

	void createGui();

	void updateData();
	void selectChat(const Chat &chat);

private slots:
	void currentTabChanged(int newTabIndex);
	void storageChanged(HistoryStorage *historyStorage);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	static HistoryWindow * instance();
	static void show(const Chat &chat);

	virtual ~HistoryWindow();

};

#endif // HISTORY_WINDOW_H
