/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_EDIT_WIDGET_H
#define ACCOUNT_EDIT_WIDGET_H

#include <QtGui/QWidget>

#include "exports.h"

class Account;

class KADUAPI AccountEditWidget : public QWidget
{
	Q_OBJECT

	Account *MyAccount;

protected:
	Account *account() { return MyAccount; }

public:
	explicit AccountEditWidget(Account *account, QWidget *parent = 0) :
			QWidget(parent), MyAccount(account) {}
	virtual ~AccountEditWidget() {}

	virtual void apply() = 0;

};

#endif // ACCOUNT_EDIT_WIDGET_H
