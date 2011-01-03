/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef ADD_BUDDY_WINDOW_H
#define ADD_BUDDY_WINDOW_H

#include <QtGui/QDialog>

#include "accounts/account.h"
#include "buddies/buddy.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QRegExpValidator;

class AccountsComboBox;
class GroupsComboBox;
class IdValidityFilter;
class SelectBuddyComboBox;

class AddBuddyWindow : public QDialog
{
	Q_OBJECT

	QLabel *UserNameLabel;
	QLineEdit *UserNameEdit;
	QAction *MobileAccountAction; // TODO: hack
	AccountsComboBox *AccountCombo;
	IdValidityFilter *AccountComboIdFilter;
	GroupsComboBox *GroupCombo;
	QLineEdit *DisplayNameEdit;
	QCheckBox *MergeContact;
	SelectBuddyComboBox *SelectContact;
	QCheckBox *AskForAuthorization;
	QCheckBox *AllowToSeeMeCheck;
	QLabel *ErrorLabel;
	QPushButton *AddContactButton;

	Buddy MyBuddy;

	void createGui();
	void addMobileAccountToComboBox();
	void displayErrorMessage(const QString &message);

	bool isMobileAccount();

	void updateAccountGui();
	void updateMobileGui();

	void validateData();
	void validateMobileData();

	bool addContact();
	bool addMobile();

	void askForAuthorization(Contact contact);

private slots:
    void accountChanged(Account account, Account lastAccount);
	void updateGui();
	void setAddContactEnabled();
	void setAccountFilter();

protected slots:
	virtual void accept();
	virtual void reject();

public:
	AddBuddyWindow(QWidget *parent = 0);
	virtual ~AddBuddyWindow();

	void setBuddy(Buddy buddy);
	void setGroup(Group group);

};

#endif // ADD_BUDDY_WINDOW_H
