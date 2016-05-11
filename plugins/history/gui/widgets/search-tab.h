/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/history-tab.h"

#include <QtCore/QDate>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QAbstractButton;
class QCheckBox;
class QDateEdit;
class QLineEdit;
class QRadioButton;
class QSplitter;

class ChatStorage;
class HistoryMessagesStorage;
class HistoryTalkableComboBox;
class PluginInjectedFactory;
class TalkableConverter;
class TimelineChatMessagesView;

class SearchTab : public HistoryTab
{
	Q_OBJECT

	QPointer<ChatStorage> m_chatStorage;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<TalkableConverter> m_talkableConverter;

	HistoryMessagesStorage *m_historyChatStorage;
	HistoryMessagesStorage *StatusStorage;
	HistoryMessagesStorage *SmsStorage;
	HistoryMessagesStorage **SearchedStorage;

	QSplitter *Splitter;
	TimelineChatMessagesView *TimelineView;

	QLineEdit *Query;

	QRadioButton *SearchInChats;
	HistoryTalkableComboBox *SelectChat;

	QRadioButton *SearchInStatuses;
	HistoryTalkableComboBox *SelectStatusBuddy;

	QRadioButton *SearchInSmses;
	HistoryTalkableComboBox *SelectSmsRecipient;

	QCheckBox *SearchByDate;
	QDateEdit *FromDate;
	QDateEdit *ToDate;

	void createGui();

private slots:
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);
	INJEQT_INIT void init();

	void kindChanged(QAbstractButton *button);
	void fromDateChanged(const QDate &date);
	void toDateChanged(const QDate &date);
	void performSearch();
	void clearSelect();

	void currentDateChanged();
	void messagesDisplayed();

public:
	explicit SearchTab(QWidget *parent = nullptr);
	virtual ~SearchTab();

	void setHistoryChatStorage(HistoryMessagesStorage *storage);
	void setStatusStorage(HistoryMessagesStorage *storage);
	void setSmsStorage(HistoryMessagesStorage *storage);

	virtual QList<int> sizes() const;
	virtual void setSizes(const QList<int> &newSizes);

};
