/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_CREATE_WIDGET_H
#define ACCOUNT_CREATE_WIDGET_H

#include <QtGui/QWidget>

#include "accounts/account.h"

class AccountCreateWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AccountCreateWidget(QWidget *parent = 0) : QWidget(parent) {}
	virtual ~AccountCreateWidget() {}

signals:
	void accountCreated(Account *account);

};

#endif // ACCOUNT_CREATE_WIDGET_H
