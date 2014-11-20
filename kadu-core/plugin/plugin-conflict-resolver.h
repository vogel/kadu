/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#pragma once

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <set>
#include <injeqt/injeqt.h>

class PluginDependencyHandler;

class KADUAPI PluginConflictResolver : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit PluginConflictResolver(QObject *parent = nullptr);
	virtual ~PluginConflictResolver();

	std::set<QString> conflictingPlugins(const std::set<QString> &activePluginSet, const QString &pluginName);

private:
	QPointer<PluginDependencyHandler> m_pluginDependencyHandler;

public slots: // TODO: make private, public only because of tests, should be done by injector
	INJEQT_SETTER void setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler);

};
