/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_MESSAGE_FILTER_H
#define ENCRYPTION_NG_SIMLITE_MESSAGE_FILTER_H

#include "contacts/contact.h"
#include "message/message-filter.h"

class EncryptionNgSimliteMessageFilter : public MessageFilter
{
	Q_OBJECT

public:
	explicit EncryptionNgSimliteMessageFilter(QObject *parent = 0);
	virtual ~EncryptionNgSimliteMessageFilter();

	virtual bool acceptMessage(const Message &message);

signals:
	void keyReceived(const Contact &contact, const QString &keyType, const QByteArray &key);

};

#endif // ENCRYPTION_NG_SIMLITE_MESSAGE_FILTER_H
