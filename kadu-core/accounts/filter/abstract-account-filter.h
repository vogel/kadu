 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACT_ACCOUNT_FILTER
#define ABSTRACT_ACCOUNT_FILTER

#include <QtCore/QObject>

class Account;

class AbstractAccountFilter : public QObject
{
	Q_OBJECT

public:
	AbstractAccountFilter(QObject *parent = 0)
			: QObject(parent) {}

	virtual bool acceptAccount(Account *account) = 0;

signals:
	void filterChanged();

};

#endif // ABSTRACT_ACCOUNT_FILTER
