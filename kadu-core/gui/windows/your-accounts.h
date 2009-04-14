/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef YOUR_ACCOUNTS
#define YOUR_ACCOUNTS

#include <QtGui/QWidget>

#include "exports.h"

KADUAPI class YourAccounts : public QWidget
{
	Q_OBJECT

	void createGui();

public:
	explicit YourAccounts(QWidget *parent = 0);
	virtual ~YourAccounts();

};

#endif // YOUR_ACCOUNT
