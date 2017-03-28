/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QIcon>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "accounts-model.h"

AccountsModel::AccountsModel(AccountManager *accountManager, QObject *parent)
        : QAbstractListModel{parent}, m_accountManager{accountManager}, m_includeIdInDisplay{}
{
    connect(m_accountManager, SIGNAL(accountUpdated(Account)), this, SLOT(accountUpdated(Account)));
    connect(m_accountManager, SIGNAL(accountAboutToBeAdded(Account)), this, SLOT(accountAboutToBeAdded(Account)));
    connect(m_accountManager, SIGNAL(accountAdded(Account)), this, SLOT(accountAdded(Account)));
    connect(m_accountManager, SIGNAL(accountAboutToBeRemoved(Account)), this, SLOT(accountAboutToBeRemoved(Account)));
    connect(m_accountManager, SIGNAL(accountRemoved(Account)), this, SLOT(accountRemoved(Account)));
}

AccountsModel::~AccountsModel()
{
}

void AccountsModel::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

int AccountsModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 1;
}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_accountManager->count();
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
    Account acc = account(index);
    if (acc.isNull())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        if (m_includeIdInDisplay)
            return QString("%1 (%2)").arg(acc.accountIdentity().name(), acc.id());
        else
            return acc.accountIdentity().name();
    case Qt::DecorationRole:
        return acc.protocolHandler() ? m_iconsManager->iconByPath(acc.protocolHandler()->icon()) : QVariant();

    case AccountRole:
        return QVariant::fromValue<Account>(acc);

    case ItemTypeRole:
        return AccountRole;

    default:
        return QVariant();
    }
}

Account AccountsModel::account(const QModelIndex &index) const
{
    if (!index.isValid())
        return Account::null;

    if (index.row() < 0 || index.row() >= rowCount())
        return Account::null;

    return m_accountManager->byIndex(index.row());
}

int AccountsModel::accountIndex(Account account) const
{
    return m_accountManager->indexOf(account);
}

QModelIndexList AccountsModel::indexListForValue(const QVariant &value) const
{
    QModelIndexList result;

    const int i = accountIndex(value.value<Account>());
    if (-1 != i)
        result.append(index(i));

    return result;
}

void AccountsModel::accountUpdated(Account account)
{
    const QModelIndexList &indexes = indexListForValue(account);
    foreach (const QModelIndex &index, indexes)
        emit dataChanged(index, index);
}

void AccountsModel::accountAboutToBeAdded(Account account)
{
    Q_UNUSED(account)

    int count = rowCount();
    beginInsertRows(QModelIndex(), count, count);
}

void AccountsModel::accountAdded(Account account)
{
    Q_UNUSED(account)

    endInsertRows();
}

void AccountsModel::accountAboutToBeRemoved(Account account)
{
    int index = accountIndex(account);
    beginRemoveRows(QModelIndex(), index, index);
}

void AccountsModel::accountRemoved(Account account)
{
    Q_UNUSED(account)

    endRemoveRows();
}

void AccountsModel::setIncludeIdInDisplay(bool includeIdInDisplay)
{
    if (m_includeIdInDisplay == includeIdInDisplay)
        return;

    m_includeIdInDisplay = includeIdInDisplay;
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

#include "moc_accounts-model.cpp"
