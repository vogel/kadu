/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MODEL_CHAIN_H
#define MODEL_CHAIN_H

#include <QtCore/QObject>

class QAbstractItemModel;
class QAbstractProxyModel;
class QModelIndex;

class KaduAbstractModel;

class ModelChain : public QObject
{
	Q_OBJECT

	QAbstractItemModel *Model;
	QList<QAbstractProxyModel *> ProxyModels;

	KaduAbstractModel *KaduModel;

public:
	explicit ModelChain(QAbstractItemModel *model, QObject *parent = 0);
	virtual ~ModelChain();

	void addProxyModel(QAbstractProxyModel *proxyModel);

	QAbstractItemModel * firstModel() const;
	QAbstractItemModel * lastModel() const;

	QModelIndex indexForValue(const QVariant &value) const;

};

#endif // MODEL_CHAIN_H
