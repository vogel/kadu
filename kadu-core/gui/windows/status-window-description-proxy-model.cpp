/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QRegExp>

#include "misc/string-utils.h"
#include "model/roles.h"

#include "status-window-description-proxy-model.h"

#define DESCRIPTION_SELECT_DISPLAY_MAX_LENGTH 60

StatusWindowDescriptionProxyModel::StatusWindowDescriptionProxyModel(QObject * parent) :
		QSortFilterProxyModel(parent)
{
}

StatusWindowDescriptionProxyModel::~StatusWindowDescriptionProxyModel()
{
}

QVariant StatusWindowDescriptionProxyModel::data(const QModelIndex &index, int role) const
{
	if (Qt::DisplayRole != role)
		return QSortFilterProxyModel::data(index, role);

	QString text = QSortFilterProxyModel::data(index, DescriptionRole).toString();
	// it'd be easier to type QString::fromUtf8(" \u21B5 ") but it doesn't work in MSVC...
	QChar arrow21b5[] = { QChar(' '), QChar(0xb5, 0x21), QChar(' '), QChar(0) };
	text = text.replace('\n', QString(arrow21b5));
	text = text.replace(QRegExp(" +"), " ");
	text = text.trimmed();
	text = StringUtils::ellipsis(text, DESCRIPTION_SELECT_DISPLAY_MAX_LENGTH);
	return QVariant(text);
}

#include "moc_status-window-description-proxy-model.cpp"
