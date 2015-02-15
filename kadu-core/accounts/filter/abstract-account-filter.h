/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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

	virtual bool acceptAccount(Account account) = 0;

signals:
	void filterChanged();

};

#endif // ABSTRACT_ACCOUNT_FILTER
