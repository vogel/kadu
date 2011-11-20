/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef PROTOCOLS_MODEL
#define PROTOCOLS_MODEL

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "model/kadu-abstract-model.h"

class ProtocolFactory;

class ProtocolsModel : public QAbstractListModel, public KaduAbstractModel
{
	Q_OBJECT

private slots:
	void protocolFactoryAboutToBeRegistered(ProtocolFactory *protocolFactory);
	void protocolFactoryRegistered(ProtocolFactory *protocolFactory);
	void protocolFactoryAboutToBeUnregistered(ProtocolFactory *protocolFactory);
	void protocolFactoryUnregistered(ProtocolFactory *protocolFactory);

public:
	explicit ProtocolsModel(QObject *parent = 0);
	virtual ~ProtocolsModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;

	ProtocolFactory * protocolFactory(const QModelIndex &index) const;
	int protocolFactoryIndex(ProtocolFactory *protocolFactory) const;
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

};

#endif // PROTOCOLS_MODEL
