/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef PROTOCOLS_MODEL_PROXY
#define PROTOCOLS_MODEL_PROXY

#include <QtGui/QSortFilterProxyModel>

class AbstractProtocolFilter;
class ProtocolsModel;

class ProtocolsModelProxy : public QSortFilterProxyModel
{
	Q_OBJECT

	ProtocolsModel *SourceProtocolModel;
	QList<AbstractProtocolFilter *> ProtocolFilters;

	bool BrokenStringCompare;
	int compareNames(QString n1, QString n2) const;
	
private slots:
	void modelDestroyed();

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	explicit ProtocolsModelProxy(QObject *parent = 0);
	virtual ~ProtocolsModelProxy();

	virtual void setSourceModel(QAbstractItemModel *sourceModel);

	void addFilter(AbstractProtocolFilter *filter);
	void removeFilter(AbstractProtocolFilter *filter);
};

#endif // PROTOCOLS_MODEL_PROXY
