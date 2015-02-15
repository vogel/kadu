/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#pragma once

#include "plugin/metadata/plugin-metadata.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QSet>
#include <QtCore/QVector>

class PluginModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum ExtraRoles
	{
		MetadataRole = 0x09386561,
		NameRole = 0x0CBBBB00,
		CommentRole = 0x19FC6DE2
	};

	explicit PluginModel(QObject *parent = nullptr);
	virtual ~PluginModel();

	virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex{}) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual int rowCount(const QModelIndex &parent = QModelIndex{}) const;

	void setPluginEntries(QVector<PluginMetadata> pluginEntries);
	void setActivePlugins(QSet<QString> activePlugins);
	const QSet<QString> & activePlugins() const;

private:
	QVector<PluginMetadata> m_pluginEntries;
	QSet<QString> m_activePlugins;

};
