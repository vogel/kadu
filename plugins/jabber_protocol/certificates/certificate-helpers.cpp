/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtDebug>
#include <QtCrypto>
#include <QStringList>
#include <QDomDocument>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "xmpp.h"

#include "misc/path-conversion.h"

#include "certificates/certificate-helpers.h"
#include "certificates/certificate-error-dialog.h"
#include "certificates/trusted-certificates-manager.h"
#include "client/mini-client.h"
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

bool CertificateHelpers::checkCertificate(QCA::TLS* tls, XMPP::QCATLSHandler *tlsHandler, QString &tlsOverrideDomain, const QString &title, const QString &host, QObject *parent)
{
	if (tls->peerCertificateChain().isEmpty())
		return false;

	QCA::Certificate cert = tls->peerCertificateChain().primary();
	int result = tls->peerIdentityResult();
	QString hostnameOverrideable;

	if (result == QCA::TLS::Valid && !tlsHandler->certMatchesHostname())
	{
		QList<QString> lst = cert.subjectInfo().values(QCA::CommonName);
		if (lst.size() == 1)
			hostnameOverrideable = lst[0];
		if (lst.size() != 1 || lst[0].isEmpty() || lst[0] != tlsOverrideDomain)
			result = QCA::TLS::HostMismatch;
	}

	// if this cert equals the user trusted certificate, just trust the user's choice.
	if (result != QCA::TLS::Valid && TrustedCertificatesManager::instance()->isTrusted(cert.toDER().toBase64()))
		result = QCA::TLS::Valid;

	if (result != QCA::TLS::Valid)
	{
		CertificateErrorDialog *errorDialog = new CertificateErrorDialog(
				title, host, cert,
				result, tls->peerCertificateValidity(),
				hostnameOverrideable, tlsOverrideDomain);

		QObject::connect(parent, SIGNAL(disconnected(Account)), errorDialog, SLOT(disconnected(Account)));
		int res = errorDialog->exec();
		
		delete errorDialog;
		
		return res == QDialog::Accepted;
	}
	else
		return true;
}

QStringList CertificateHelpers::getCertificateStoreDirs()
{
	QStringList l;
	l += profilePath("certs");
	return l;
}

QString CertificateHelpers::getCertificateStoreSaveDir()
{
	QDir certsave(profilePath("certs"));
	if (!certsave.exists())
	{
		QDir home(profilePath());
		home.mkdir("certs");
	}

	return certsave.path();
}
