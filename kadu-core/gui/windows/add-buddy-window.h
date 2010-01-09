/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ADD_BUDDY_WINDOW_H
#define ADD_BUDDY_WINDOW_H

#include <QtGui/QDialog>

#include "buddies/buddy.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QRegExpValidator;

class AccountsComboBox;
class GroupsComboBox;
class IdRegularExpressionFilter;
class SelectBuddyCombobox;

class AddBuddyWindow : public QDialog
{
	Q_OBJECT

	QLabel *UserNameLabel;
	QLineEdit *UserNameEdit;
	QRegExpValidator *UserNameValidator;
	AccountsComboBox *AccountCombo;
	IdRegularExpressionFilter *AccountComboIdFilter;
	GroupsComboBox *GroupCombo;
	QLineEdit *DisplayNameEdit;
	QCheckBox *MergeContact;
	SelectBuddyCombobox *SelectContact;
	QCheckBox *AllowToSeeMeCheck;
	QLabel *ErrorLabel;
	QPushButton *AddContactButton;

	Buddy MyBuddy;

	void createGui();
	void displayErrorMessage(const QString &message);

private slots:
	void setUsernameLabel();
	void setAddContactEnabled();
	void setValidateRegularExpression();
	void setAccountFilter();

protected slots:
	virtual void accept();
	virtual void reject();

public:
	AddBuddyWindow(QWidget *parent = 0);
	virtual ~AddBuddyWindow();

	void setBuddy(Buddy buddy);

};

#endif // ADD_BUDDY_WINDOW_H
