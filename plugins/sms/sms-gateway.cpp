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

#include "sms-gateway.h"

SmsGateway::SmsGateway() :
		MaxLength(0), SignatureRequired(true)
{
}

SmsGateway::SmsGateway(const SmsGateway &copyMe)
{
	Name = copyMe.Name;
	Id = copyMe.Id;
	MaxLength = copyMe.MaxLength;
	SignatureRequired = copyMe.SignatureRequired;
}

SmsGateway & SmsGateway::operator = (const SmsGateway &copyMe)
{
	Name = copyMe.Name;
	Id = copyMe.Id;
	MaxLength = copyMe.MaxLength;
	SignatureRequired = copyMe.SignatureRequired;

	return *this;
}

void SmsGateway::setName(const QString &name)
{
	Name = name;
}

QString SmsGateway::name() const
{
	return Name;
}

void SmsGateway::setId(const QString &id)
{
	Id = id;
}

QString SmsGateway::id() const
{
	return Id;
}

void SmsGateway::setMaxLength(const quint16 maxLength)
{
	MaxLength = maxLength;
}

quint16 SmsGateway::maxLength() const
{
	return MaxLength;
}

void SmsGateway::setSignatureRequired(const bool signatureRequired)
{
	SignatureRequired = signatureRequired;
}

bool SmsGateway::signatureRequired() const
{
	return SignatureRequired;
}
