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

#include <QtGui/QAbstractItemView>

#include "gui/widgets/buddies-list-view-item-painter.h"

#include "kadu-tree-view-delegate.h"

KaduTreeViewDelegate::KaduTreeViewDelegate(QObject *parent) :
		QItemDelegate(parent), Configuration(parent), UseConfigurationColors(false)
{
}

KaduTreeViewDelegate::~KaduTreeViewDelegate()
{
}

void KaduTreeViewDelegate::setShowAccountName(bool showAccountName)
{
	Configuration.setShowAccountName(showAccountName);
}

QStyleOptionViewItemV4 KaduTreeViewDelegate::getOptions(const QModelIndex &index, const QStyleOptionViewItem &option) const
{
	QStyleOptionViewItemV4 opt = setOptions(index, option);

	const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
	opt.features = v2
		? v2->features
		: QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
	const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
	opt.locale = v3 ? v3->locale : QLocale();
	opt.widget = v3 ? v3->widget : 0;

	return opt;
}

QSize KaduTreeViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	BuddiesListViewItemPainter buddyPainter(Configuration, getOptions(index, option), index, UseConfigurationColors);
	return QSize(0, buddyPainter.height());
}

void KaduTreeViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 options = getOptions(index, option);

	const QAbstractItemView *widget = qobject_cast<const QAbstractItemView *>(options.widget);
	if (!widget)
		return;

	QStyle *style = widget->style();
	style->drawControl(QStyle::CE_ItemViewItem, &options, painter, widget);

	BuddiesListViewItemPainter buddyPainter(Configuration, options, index, UseConfigurationColors);
	buddyPainter.paint(painter);
}

