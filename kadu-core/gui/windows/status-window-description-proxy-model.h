/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_WINDOW_DESCRIPTION_PROXY_MODEL_H
#define STATUS_WINDOW_DESCRIPTION_PROXY_MODEL_H

#include <QtCore/QSortFilterProxyModel>

/**
 * @class StatusWindowDescriptionProxyModel
 * @author Piotr 'ultr' Dąbrowski
 * @short Proxy model for descriptions.
 *
 * Proxy model for descriptions. Displays every description in a single line with new line characters replaced with a nice indicator.
 */
class StatusWindowDescriptionProxyModel : public QSortFilterProxyModel // TODO: replace with QIdentityProxyModel from Qt 4.8
{
	Q_OBJECT

public:
	explicit StatusWindowDescriptionProxyModel(QObject * parent = 0);
	virtual ~StatusWindowDescriptionProxyModel();

	virtual QVariant data(const QModelIndex &index, int role) const;

};

#endif // STATUS_WINDOW_DESCRIPTION_PROXY_MODEL_H
