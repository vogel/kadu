 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Copyright (C) 2008  Remko Troncon
 * Licensed under the GNU GPL license.
 * See COPYING for details.
 */

#ifndef CERTUTIL_H
#define CERTUTIL_H

#include <QtCrypto>

class JabberAccount;
class QString;
namespace QCA {
	class CertificateCollection;
}
namespace XMPP {
	class QCATLSHandler;
}

class CertificateHelpers 
{
	public:
		static QCA::CertificateCollection allCertificates(const QStringList& dirs);
		static QString resultToString(int result, QCA::Validity);
		static bool checkCertificate(QCA::TLS* tls, XMPP::QCATLSHandler *tlsHandler, QString &tlsOverrideDomain, QByteArray &tlsOverrideCert, const QString &title, const QString &host, QObject *parent);
		static QStringList getCertificateStoreDirs();
		static QString getCertificateStoreSaveDir();

	protected:
		static QString validityToString(QCA::Validity);
};

#endif
