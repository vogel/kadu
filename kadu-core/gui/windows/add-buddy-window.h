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

#include "contacts/contact.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QRegExpValidator;

class AccountsModel;

class AddBuddyWindow : public QDialog
{
	Q_OBJECT

	QLineEdit *UserNameEdit;
	QRegExpValidator *UserNameValidator;
	QComboBox *AccountCombo;
	AccountsModel *AccountComboModel;
	QComboBox *GroupCombo;
	QLineEdit *DisplayNameEdit;
	QCheckBox *AllowToSeeMeCheck;
	QPushButton *AddContactButton;

	Contact MyContact;

	void createGui();

	Account * selectedAccount();

private slots:
	void setAddContactEnabled();
	void setValidateRegularExpression();

protected slots:
	virtual void accept();
	virtual void reject();

public:
	AddBuddyWindow(QWidget *parent = 0);
	virtual ~AddBuddyWindow();

	void setContact(Contact contact);

};

#endif // ADD_BUDDY_WINDOW_H
