/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QtCrypto>
#include <QtDebug>

#include "xmpp.h"

#include "misc/kadu-paths.h"

#include "certificates/certificate-helpers.h"
#include "certificates/trusted-certificates-manager.h"
#include "client/mini-client.h"
#include "gui/windows/certificate-error-window.h"
#include "jabber-protocol.h"

using namespace QCA;

/**
 * \class CertificateHelpers
 * \brief A class providing utility functions for Certificates.
 */

/**
 * \brief Returns the collection of all available certificates.
 * This collection includes the system-wide certificates, as well as any
 * custom certificate in the Psi-specific cert dirs.
 */
CertificateCollection CertificateHelpers::allCertificates(const QStringList& storeDirs)
{
	CertificateCollection certs(systemStore());
	for (QStringList::ConstIterator s = storeDirs.begin(); s != storeDirs.end(); ++s)
	{
		QDir store(*s);

		// Read in PEM certificates
		store.setNameFilters(QStringList("*.crt") + QStringList("*.pem"));
		QStringList cert_files = store.entryList();
		for (QStringList::ConstIterator c = cert_files.begin(); c != cert_files.end(); ++c)
		{
			//qDebug() << "certutil.cpp: Reading " << store.filePath(*c);
			ConvertResult result;
			Certificate cert = Certificate::fromPEMFile(store.filePath(*c),&result);
			if (result == ConvertGood)
				certs.addCertificate(cert);
			else
				qWarning() << QString("certutil.cpp: Invalid PEM certificate: %1").arg(store.filePath(*c));
		}

		// Read in old XML format certificates (DEPRECATED)
		store.setNameFilters(QStringList("*.xml"));
		cert_files = store.entryList();
		for (QStringList::ConstIterator it = cert_files.begin(); it != cert_files.end(); ++it)
		{
			qWarning() << "Loading certificate in obsolete XML format: " << store.filePath(*it);
			QFile f(store.filePath(*it));
			if (!f.open(QIODevice::ReadOnly))
				continue;
			QDomDocument doc;
			bool ok = doc.setContent(&f);
			f.close();
			if (!ok)
				continue;

			QDomElement base = doc.documentElement();
			if (base.tagName() != "store")
				continue;
			QDomNodeList cl = base.elementsByTagName("certificate");

			for (int n = 0; n < (int)cl.count(); ++n)
			{
				QDomElement data = cl.item(n).toElement().elementsByTagName("data").item(0).toElement();
				if (!data.isNull())
				{
					ConvertResult result;
					Certificate cert = Certificate::fromDER(Base64().stringToArray(data.text()).toByteArray(),&result);
					if (result == ConvertGood)
						certs.addCertificate(cert);
					else
						qWarning() << "certificate-helpers.cpp: Invalid XML certificate: %1" << store.filePath(*it);
				}
			}
		}
	}
	return certs;
}

QString CertificateHelpers::validityToString(QCA::Validity v)
{
	QString s;
	switch (v)
	{
		case QCA::ValidityGood:
			s = "Validated";
			break;
		case QCA::ErrorRejected:
			s = "Root CA is marked to reject the specified purpose";
			break;
		case QCA::ErrorUntrusted:
			s = "Certificate not trusted for the required purpose";
			break;
		case QCA::ErrorSignatureFailed:
			s = "Invalid signature";
			break;
		case QCA::ErrorInvalidCA:
			s = "Invalid CA certificate";
			break;
		case QCA::ErrorInvalidPurpose:
			s = "Invalid certificate purpose";
			break;
		case QCA::ErrorSelfSigned:
			s = "Certificate is self-signed";
			break;
		case QCA::ErrorRevoked:
			s = "Certificate has been revoked";
			break;
		case QCA::ErrorPathLengthExceeded:
			s = "Maximum certificate chain length exceeded";
			break;
		case QCA::ErrorExpired:
			s = "Certificate has expired";
			break;
		case QCA::ErrorExpiredCA:
			s = "CA has expired";
			break;
		case QCA::ErrorValidityUnknown:
		default:
			s = "General certificate validation error";
			break;
	}
	return s;
}

QString CertificateHelpers::resultToString(int result, QCA::Validity validity)
{
	QString s;
	switch (result)
	{
		case QCA::TLS::NoCertificate:
			s = QObject::tr("The server did not present a certificate.");
			break;
		case QCA::TLS::Valid:
			s = QObject::tr("Certificate is valid.");
			break;
		case QCA::TLS::HostMismatch:
			s = QObject::tr("The hostname does not match the one the certificate was issued to.");
			break;
		case QCA::TLS::InvalidCertificate:
			s = validityToString(validity);
			break;

		default:
			s = QObject::tr("General certificate validation error.");
			break;
	}
	return s;
}

bool CertificateHelpers::checkCertificate(QCA::TLS* tls, XMPP::QCATLSHandler *tlsHandler, QString &tlsOverrideDomain, const QString &title, const QString &host, bool blocking, QObject *receiver, const char *slot)
{
	if (!tlsHandler || !tls || tls->peerCertificateChain().isEmpty())
		return false;

	QCA::Certificate certificate = tls->peerCertificateChain().primary();

	if (TrustedCertificatesManager::instance()->isTrusted(certificate.toDER().toBase64()))
		return true;

	int result = tls->peerIdentityResult();
	QString overridenHostname;

	if (result == QCA::TLS::Valid)
		return true;

	if (false == tlsHandler->certMatchesHostname())
	{
		QList<QString> subjectInfo = certificate.subjectInfo().values(QCA::CommonName);
		if (subjectInfo.size() == 1)
			overridenHostname = subjectInfo[0];
		if (subjectInfo.size() != 1 || subjectInfo[0].isEmpty() || subjectInfo[0] != tlsOverrideDomain)
			result = QCA::TLS::HostMismatch;
	}

	if (blocking)
		Q_ASSERT(!receiver && !slot);
	else
		Q_ASSERT(receiver && slot);

	CertificateErrorWindow *errorDialog = new CertificateErrorWindow(
			title, host, certificate,
			result, tls->peerCertificateValidity(),
			overridenHostname, receiver, slot);

	if (blocking)
		return QDialog::Accepted == errorDialog->exec();

	errorDialog->show();
	return false;
}

QStringList CertificateHelpers::getCertificateStoreDirs()
{
	QStringList l;
	l += KaduPaths::instance()->profilePath() + QLatin1String("certs");
	return l;
}
