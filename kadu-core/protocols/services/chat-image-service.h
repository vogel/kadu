/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account.h"
#include "exports.h"

class KADUAPI ChatImageService : public QObject
{
	Q_OBJECT

public:
	static const int NoSizeLimit = -1;

	static QString imagesPath();

	explicit ChatImageService(QObject *parent);

	virtual qint64 hardSizeLimit() { return NoSizeLimit; }
	virtual qint64 softSizeLimit() { return NoSizeLimit; }
	bool fitsHardSizeLimit(qint64 size);
	bool fitsSoftSizeLimit(qint64 size);
	virtual bool showSoftSizeWarning(Account account) { Q_UNUSED(account); return true; }

signals:
	void imageReceived(const QString &id, const QString &fileName);

};

// for MOC
#include <QtCore/QString>

#endif // CHAT_IMAGE_SERVICE_H
