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

#include <QtGui/QWidget>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;

class Account;
class Contact;
class Group;

class AddBuddyWindow : public QWidget
{
	Q_OBJECT

	Account *contactAccount;

	QLineEdit *buddyUid;
	QPushButton *continueButton;
	QWidget *chooseAccountWidget;
	QComboBox *chooseAccountCombo;
	QCheckBox *offlineToCheckBox;
	QWidget *stepTwoWidget;
	QComboBox *chooseNameCombo;
	QComboBox *addToGroupCombo;
	QWidget *addToGroupWidget;
	QLineEdit *newGroupNameEdit;
	QPushButton *expandMoreOptionsButton;
	QPushButton *addBuddyButton;

	bool showMoreOptions;

	void createGui();

private slots:
	void helpLinkClicked();
	void clearUid();
	void buddyIdTextChanged(QString id);
	void stepOneFinished();
	void accountSelected(int index);
	void groupSelected(int index);
	void showMoreOptionsChanged();
	void addBuddy();

public:
	AddBuddyWindow(QWidget *parent = 0);
	AddBuddyWindow(Contact contact, QWidget *parent = 0);
	AddBuddyWindow(Group *group, QWidget *parent = 0);
	~AddBuddyWindow();
};

#endif // ADD_BUDDY_WINDOW_H
