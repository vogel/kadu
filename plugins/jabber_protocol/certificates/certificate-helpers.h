/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

/*
 * Copyright (C) 2008  Remko Troncon
 * Licensed under the GNU GPL license.
 * See COPYING for details.
 */

#ifndef CERTUTIL_H
#define CERTUTIL_H

#include <QtCrypto>

class JabberAccountDetails;
class QString;
namespace QCA
{
	class CertificateCollection;
}

namespace XMPP
{
	class QCATLSHandler;
}

class CertificateHelpers 
{
	public:
		static QCA::CertificateCollection allCertificates(const QStringList& dirs);
		static QString resultToString(int result, QCA::Validity);
		static bool checkCertificate(QCA::TLS* tls, XMPP::QCATLSHandler *tlsHandler, QString &tlsOverrideDomain, const QString &title, const QString &host, QObject *parent);
		static QStringList getCertificateStoreDirs();
		static QString getCertificateStoreSaveDir();

	protected:
		static QString validityToString(QCA::Validity);
};

#endif
