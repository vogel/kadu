/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "protocols/filter/abstract-protocol-filter.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "protocols-model-proxy.h"
#include "protocols-model.h"

ProtocolsModelProxy::ProtocolsModelProxy(QObject *parent) :
		QSortFilterProxyModel(parent)
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

int ProtocolsModelProxy::compareNames(QString n1, QString n2) const
{
	return BrokenStringCompare
		? n1.toLower().localeAwareCompare(n2.toLower())
		: n1.localeAwareCompare(n2);
}

bool ProtocolsModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	if (!sourceModel())
		return QSortFilterProxyModel::lessThan(left, right);

	QVariant lVariant = sourceModel()->data(left, ProtocolRole);
	QVariant rVariant = sourceModel()->data(right, ProtocolRole);

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
	if (!protocol)
		return true;

	foreach (AbstractProtocolFilter *filter, ProtocolFilters)
		if (!filter->acceptProtocol(protocol))
			return false;

	return true;
}

void ProtocolsModelProxy::addFilter(AbstractProtocolFilter *filter)
{
	if (ProtocolFilters.contains(filter))
		return;

	ProtocolFilters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void ProtocolsModelProxy::removeFilter(AbstractProtocolFilter *filter)
{
	if (ProtocolFilters.removeAll(filter) <= 0)
		return;

	invalidateFilter();
	disconnect(filter, 0, this, 0);
}

#include "moc_protocols-model-proxy.cpp"
