/*
 * %kadu copyright begin%
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

#ifndef SMS_GATEWAY_H
#define SMS_GATEWAY_H

#include <QtCore/QString>

class SmsGateway
{
	QString Name;
	QString Id;
	quint16 MaxLength;
	bool SignatureRequired;

public:
	SmsGateway();
	SmsGateway(const SmsGateway &copyMe);

	SmsGateway & operator = (const SmsGateway &copyMe);

	void setName(const QString &name);
	QString name() const;

	void setId(const QString &id);
	QString id() const;

	void setMaxLength(const quint16 maxLength);
	quint16 maxLength() const;

	void setSignatureRequired(const bool signatureRequired);
	bool signatureRequired() const;

};

#endif // SMS_GATEWAY_H
