/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "model/kadu-abstract-model.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class IdentityManager;
class Identity;

class IdentityModel : public QAbstractListModel, public KaduAbstractModel
{
    Q_OBJECT

public:
    explicit IdentityModel(QObject *parent = nullptr);
    virtual ~IdentityModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role) const;

    Identity identity(const QModelIndex &index) const;
    int identityIndex(Identity identity) const;
    virtual QModelIndexList indexListForValue(const QVariant &value) const;

private:
    QPointer<IdentityManager> m_identityManager;

private slots:
    INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
    INJEQT_INIT void init();

    void identityAboutToBeAdded(Identity identity);
    void identityAdded(Identity identity);
    void identityAboutToBeRemoved(Identity identity);
    void identityRemoved(Identity identity);
};
