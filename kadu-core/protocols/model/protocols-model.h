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

#include "model/kadu-abstract-model.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class IconsManager;
class ProtocolFactory;
class ProtocolsManager;

class ProtocolsModel : public QAbstractListModel, public KaduAbstractModel
{
    Q_OBJECT

public:
    explicit ProtocolsModel(ProtocolsManager *protocolsManager, QObject *parent = nullptr);
    virtual ~ProtocolsModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role) const;

    ProtocolFactory *protocolFactory(const QModelIndex &index) const;
    int protocolFactoryIndex(ProtocolFactory *protocolFactory) const;
    virtual QModelIndexList indexListForValue(const QVariant &value) const;

private:
    QPointer<IconsManager> m_iconsManager;
    QPointer<ProtocolsManager> m_protocolsManager;

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);

    void protocolFactoryAboutToBeRegistered(ProtocolFactory *protocolFactory);
    void protocolFactoryRegistered(ProtocolFactory *protocolFactory);
    void protocolFactoryAboutToBeUnregistered(ProtocolFactory *protocolFactory);
    void protocolFactoryUnregistered(ProtocolFactory *protocolFactory);
};
