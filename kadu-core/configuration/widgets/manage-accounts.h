/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MANAGE_ACCOUNTS_H
#define MANAGE_ACCOUNTS_H

#include <QtGui/QWidget>

class QListWidget;
class QPushButton;

class ManageAccounts : public QWidget
{
	Q_OBJECT

	QListWidget *AccountsListWidget;
	QPushButton *AddAccountButton;
	QPushButton *RemoveAccountButton;
	QPushButton *MoveUpAccountButton;
	QPushButton *MoveDownAccountButton;

public:
	ManageAccounts(QWidget *parent = 0);
	virtual ~ManageAccounts();

};

#endif // MANAGE_ACCOUNTS_H
