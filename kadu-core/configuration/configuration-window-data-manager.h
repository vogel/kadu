/*
 * %kadu copyright begin%
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIGURATION_WINDOW_DATA_MANAGER_H
#define CONFIGURATION_WINDOW_DATA_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "exports.h"

class KADUAPI ConfigurationWindowDataManager : public QObject
{
	Q_OBJECT

public:
	explicit ConfigurationWindowDataManager(QObject *parent = 0);
	virtual ~ConfigurationWindowDataManager();

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value) = 0;
	virtual QVariant readEntry(const QString &section, const QString &name) = 0;

};

#endif // CONFIGURATION_WINDOW_DATA_MANAGER_H
