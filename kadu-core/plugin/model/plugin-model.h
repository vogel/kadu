/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
#include <QtCore/QWeakPointer>

class PluginActivationService;
class PluginEntry;
class PluginListWidget;

class PluginModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum ExtraRoles
	{
		PluginEntryRole   = 0x09386561,
		NameRole          = 0x0CBBBB00,
		CommentRole       = 0x19FC6DE2
	};

	explicit PluginModel(PluginListWidget *pluginListWidget, QObject *parent = nullptr);
	virtual ~PluginModel();

	void setPluginActivationService(PluginActivationService *pluginActivationService);

	virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex{}) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual int rowCount(const QModelIndex &parent = QModelIndex{}) const;

	void loadPluginData();

private:
	QWeakPointer<PluginActivationService> m_pluginActivationService;
	PluginListWidget *m_pluginListWidget;
	QList<PluginEntry> m_pluginEntries;

};


class PluginEntry
{

public:
	QString category;
	QString name;
	QString pluginName;
	QString description;
	QString author;
	bool checked;
	bool isCheckable;

	bool operator == (const PluginEntry &pe) const
	{
		return name == pe.name;
	}

};

Q_DECLARE_METATYPE(PluginEntry*)
