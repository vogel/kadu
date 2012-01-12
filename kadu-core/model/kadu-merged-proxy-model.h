/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_MERGED_PROXY_MODEL_H
#define KADU_MERGED_PROXY_MODEL_H

#include "model/kadu-abstract-model.h"
#include "model/merged-proxy-model.h"

class KaduMergedProxyModel : public MergedProxyModel, public KaduAbstractModel
{
	Q_OBJECT

	QList<KaduAbstractModel *> Models;

public:
	explicit KaduMergedProxyModel(QObject *parent = 0);
	virtual ~KaduMergedProxyModel();

	virtual void setModels(QList<QAbstractItemModel *> models);

	virtual QModelIndexList indexListForValue(const QVariant &value) const;

};

#endif // KADU_MERGED_PROXY_MODEL_H
