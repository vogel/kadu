/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef CHAT_IMAGE_SERVICE_H
#define CHAT_IMAGE_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

class KADUAPI ChatImageService : public QObject
{
	Q_OBJECT

public:
	explicit ChatImageService(QObject *parent = 0) : QObject(parent) {}

signals:
	void imageReceived(const QString &id, const QString &fileName);

};

#endif // CHAT_IMAGE_SERVICE_H
