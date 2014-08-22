/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ADD_CONFERENCE_WINDOW_H
#define ADD_CONFERENCE_WINDOW_H

#include <QtWidgets/QDialog>

#include "buddies/buddy-set.h"

class QLabel;
class QLineEdit;

class AccountsComboBox;
class AccountTalkableFilter;
class BuddyListModel;
class Chat;
class CheckableBuddiesProxyModel;
class ModelChain;

class AddConferenceWindow : public QDialog
{
	Q_OBJECT

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
	void accountChanged();
	void validateData();

public:
	explicit AddConferenceWindow(QWidget *parent = 0);
	virtual ~AddConferenceWindow();

public slots:
	void accept();
	void start();

};

#endif // ADD_CONFERENCE_WINDOW_H
