/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "exports.h"
#include "os/generic/desktop-aware-object.h"

class QCheckBox;
class QFormLayout;
class QLabel;
class QLineEdit;
class QRegExpValidator;

class AccountsComboBox;
class GroupsComboBox;
class SelectTalkableComboBox;

class KADUAPI AddBuddyWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	QFormLayout *Layout;

	QLabel *UserNameLabel;
	QLineEdit *UserNameEdit;
	QAction *MobileAccountAction; // TODO: hack
	QAction *EmailAccountAction; // TODO: hack
	Account LastSelectedAccount;
	AccountsComboBox *AccountCombo;
	GroupsComboBox *GroupCombo;
	QLineEdit *DisplayNameEdit;
	QCheckBox *MergeBuddy;
	SelectTalkableComboBox *SelectBuddy;
	QCheckBox *AskForAuthorization;
	QCheckBox *AllowToSeeMeCheck;
	QLabel *ErrorLabel;
	QPushButton *AddContactButton;

	QList<QWidget *> NonMergeWidgets;
	QList<QWidget *> MergeWidgets;

	Buddy MyBuddy;
	Account MyAccount;
	bool ForceBuddyAccount;

	void createGui();
	void addFakeAccountsToComboBox();
	void displayErrorMessage(const QString &message);

	bool isMobileAccount();
	bool isEmailAccount();

	void updateAccountGui();
	void updateMobileGui();
	void updateEmailGui();

	void validateData();
	void validateMobileData();
	void validateEmailData();

	bool addContact();
	bool addMobile();
	bool addEmail();

	void askForAuthorization(const Contact &contact);
	void sendAuthorization(const Contact &contact);

private slots:
	void accountChanged();
	void updateGui();
	void setAddContactEnabled();
	void mergeToggled(bool toggled);

public:
	explicit AddBuddyWindow(QWidget *parent = 0, const Buddy &buddy = Buddy::null, bool forceBuddyAccount = false);
	virtual ~AddBuddyWindow();

	void setGroup(Group group);

public slots:
	virtual void accept();

};

#endif // ADD_BUDDY_WINDOW_H
