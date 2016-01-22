/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-list.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <memory>

class BuddyList;
class BuddyManager;

class QMimeData;

class BuddyListMimeDataService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit BuddyListMimeDataService(QObject *parent = nullptr);
	virtual ~BuddyListMimeDataService();

	QStringList mimeTypes();
	std::unique_ptr<QMimeData> toMimeData(const BuddyList &buddyList);
	BuddyList fromMimeData(const QMimeData *mimeData);

private:
	QPointer<BuddyManager> m_buddyManager;

	QLatin1String m_mimeType;

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);

};
