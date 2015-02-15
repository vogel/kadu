/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DESCRIPTION_MODEL
#define DESCRIPTION_MODEL

#include <QtCore/QAbstractListModel>

class DescriptionManager;

class DescriptionModel : public QAbstractListModel
{
	Q_OBJECT

	DescriptionManager *Manager;

private slots:
	void descriptionAboutToBeAdded(const QString &description);
	void descriptionAdded(const QString &description);
	void descriptionAboutToBeRemoved(const QString &description);
	void descriptionRemoved(const QString &description);

public:
	explicit DescriptionModel(DescriptionManager *manager);
	virtual ~DescriptionModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

};

#endif // DESCRIPTION_MODEL
