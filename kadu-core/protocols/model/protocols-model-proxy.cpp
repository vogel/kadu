/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <stdio.h>

#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/filter/abstract-protocol-filter.h"

#include "protocols-model.h"
#include "protocols-model-proxy.h"

ProtocolsModelProxy::ProtocolsModelProxy(QObject *parent)
	: QSortFilterProxyModel(parent), SourceProtocolModel(0)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (BrokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");
}

ProtocolsModelProxy::~ProtocolsModelProxy()
{

}

void ProtocolsModelProxy::setSourceModel(QAbstractItemModel *sourceModel)
{
	if (SourceProtocolModel)
		disconnect(SourceProtocolModel, SIGNAL(destroyed()), this, SLOT(modelDestroyed()));

	SourceProtocolModel = qobject_cast<ProtocolsModel *>(sourceModel);
	QSortFilterProxyModel::setSourceModel(SourceProtocolModel);

	if (SourceProtocolModel)
		connect(SourceProtocolModel, SIGNAL(destroyed()), this, SLOT(modelDestroyed()));

	setDynamicSortFilter(true);
	sort(0);
}

int ProtocolsModelProxy::compareNames(QString n1, QString n2) const
{
	return BrokenStringCompare
		? n1.toLower().localeAwareCompare(n2.toLower())
		: n1.localeAwareCompare(n2);
}

void ProtocolsModelProxy::modelDestroyed()
{
	SourceProtocolModel = 0;
}

bool ProtocolsModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	if (!SourceProtocolModel)
		return QSortFilterProxyModel::lessThan(left, right);

	QVariant lVariant = SourceProtocolModel->data(left, ProtocolRole);
	QVariant rVariant = SourceProtocolModel->data(right, ProtocolRole);

	if (!lVariant.canConvert<ProtocolFactory *>() || !rVariant.canConvert<ProtocolFactory *>())
		return QSortFilterProxyModel::lessThan(left, right);

	ProtocolFactory *leftProtocol = lVariant.value<ProtocolFactory *>();
	ProtocolFactory *rightProtocol = rVariant.value<ProtocolFactory *>();

	int displayCompare = compareNames(leftProtocol->name(), rightProtocol->name());
	return displayCompare < 0;
}

bool ProtocolsModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	QVariant pVariant = sourceModel()->index(sourceRow, 0, sourceParent).data(ProtocolRole);
	if (!pVariant.canConvert<ProtocolFactory *>())
		return true;

	ProtocolFactory *protocol = pVariant.value<ProtocolFactory *>();
	foreach (AbstractProtocolFilter *filter, ProtocolFilters)
		if (!filter->acceptProtocol(protocol))
			return false;

	return true;
}

void ProtocolsModelProxy::addFilter(AbstractProtocolFilter *filter)
{
	ProtocolFilters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void ProtocolsModelProxy::removeFilter(AbstractProtocolFilter *filter)
{
	ProtocolFilters.removeAll(filter);
	invalidateFilter();
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}
