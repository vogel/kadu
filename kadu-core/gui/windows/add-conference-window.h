/*
 * %kadu copyright begin%
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

#include "buddies/buddy-set.h"

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

class QLabel;
class QLineEdit;

class AccountsComboBox;
class AccountTalkableFilter;
class BuddyListModel;
class ChatManager;
class ChatWidgetManager;
class Chat;
class CheckableBuddiesProxyModel;
class InjectedFactory;
class ModelChain;

class AddConferenceWindow : public QDialog
{
	Q_OBJECT

public:
	explicit AddConferenceWindow(QWidget *parent = nullptr);
	virtual ~AddConferenceWindow();

public slots:
	void accept();
	void start();

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<InjectedFactory> m_injectedFactory;

	AccountsComboBox *AccountCombo;
	QLineEdit *DisplayNameEdit;
	QLabel *ErrorLabel;
	QPushButton *AddButton;
	QPushButton *StartButton;

	AccountTalkableFilter *AccountFilter;
	BuddyListModel *Model;
	// CheckableBuddiesProxyModel *CheckableProxy;

	void createGui();
	void displayErrorMessage(const QString &message);

	Chat computeChat() const;
	BuddySet filterByAccount(const Account &account, const BuddySet &buddies) const;

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void accountChanged();
	void validateData();

};
