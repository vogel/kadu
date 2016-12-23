/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "accounts/account.h"
#include "model/kadu-abstract-model.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountManager;
class IconsManager;

class AccountsModel : public QAbstractListModel, public KaduAbstractModel
{
	Q_OBJECT

public:
	explicit AccountsModel(AccountManager *accountManager, QObject *parent = nullptr);
	virtual ~AccountsModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;

	Account account(const QModelIndex &index) const;
	int accountIndex(Account account) const;
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

	void setIncludeIdInDisplay(bool includeIdInDisplay);

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<IconsManager> m_iconsManager;

	bool m_includeIdInDisplay;

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);

	void accountUpdated(Account account);
	void accountAboutToBeAdded(Account account);
	void accountAdded(Account account);
	void accountAboutToBeRemoved(Account account);
	void accountRemoved(Account account);

};
