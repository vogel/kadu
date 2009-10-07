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

class QCheckBox;
class QComboBox;
class QLineEdit;

class AddBuddyWindow : public QDialog
{
	Q_OBJECT

	QLineEdit *UserNameEdit;
	QComboBox *AccountCombo;
	QComboBox *GroupCombo;
	QLineEdit *DisplayNameEdit;
	QCheckBox *AllowToSeeMeCheck;

	void createGui();

public:
	AddBuddyWindow(QWidget *parent = 0);
	virtual ~AddBuddyWindow();

};

#endif // ADD_BUDDY_WINDOW_H
