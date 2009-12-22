/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_ADD_WIDGET_H
#define ACCOUNT_ADD_WIDGET_H

#include <QtGui/QWidget>

#include "accounts/account.h"

#include "exports.h"

class KADUAPI AccountAddWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AccountAddWidget(QWidget *parent = 0) : QWidget(parent) {}
	virtual ~AccountAddWidget() {}
	
	virtual void apply() = 0;

signals:
	void accountCreated(Account account);

};

#endif // ACCOUNT_ADD_WIDGET_H
