/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACCOUNTS_MODEL
#define ACCOUNTS_MODEL

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "accounts/account.h"
#include "model/kadu-abstract-model.h"

class AccountsModel : public QAbstractListModel, public KaduAbstractModel
{
	Q_OBJECT

	bool IncludeIdInDisplay;

private slots:
	void accountUpdated(Account account);
	void accountAboutToBeRegistered(Account account);
	void accountRegistered(Account account);
	void accountAboutToBeUnregistered(Account account);
	void accountUnregistered(Account account);

public:
	explicit AccountsModel(QObject *parent = 0);
	virtual ~AccountsModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Account account(const QModelIndex &index) const;
	int accountIndex(Account account) const;
	virtual QModelIndex indexForValue(const QVariant &value) const;

	void setIncludeIdInDisplay(bool includeIdInDisplay);

};

#endif // ACCOUNTS_MODEL
