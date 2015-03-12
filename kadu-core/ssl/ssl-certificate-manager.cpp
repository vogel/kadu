/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "ssl-certificate-manager.h"

#include "ssl/gui/ssl-certificate-error-dialog.h"
#include "ssl/ssl-certificate-repository.h"
#include "ssl/ssl-certificate-storage.h"
#include "ssl/ssl-certificate.h"

#include <QtNetwork/QSslCertificate>

SslCertificateManager::SslCertificateManager(QObject *parent) :
		QObject(parent)
{
}

SslCertificateManager::~SslCertificateManager()
{
}

void SslCertificateManager::setSslCertificateRepository(SslCertificateRepository *sslCertificateRepository)
{
	m_sslCertificateRepository = sslCertificateRepository;
}

void SslCertificateManager::setSslCertificateStorage(SslCertificateStorage *sslCertificateStorage)
{
	m_sslCertificateStorage = sslCertificateStorage;
}

void SslCertificateManager::loadPersistentSslCertificates()
{
	if (!m_sslCertificateStorage || !m_sslCertificateRepository)
		return;

	m_sslCertificateRepository->setPersistentCertificates(m_sslCertificateStorage->loadCertificates());
}

void SslCertificateManager::storePersistentSslCertificates()
{
	if (!m_sslCertificateStorage || !m_sslCertificateRepository)
		return;

	m_sslCertificateStorage->storeCertificates(m_sslCertificateRepository->persistentCertificates());
}

bool SslCertificateManager::acceptCertificate(const QString &hostName, const QSslCertificate &certificate, const QList<QSslError> &errors) const
{
	auto hostCertificate = SslCertificate{hostName, certificate.toPem().toHex()};
	if (m_sslCertificateRepository->containsCertificate(hostCertificate))
		return true;

	auto sslCertificateErrorDialog = new SslCertificateErrorDialog{hostCertificate, errors};
	sslCertificateErrorDialog->setSslCertificateRepository(m_sslCertificateRepository);
	return QDialog::Accepted == sslCertificateErrorDialog->exec();
}

#include "moc_ssl-certificate-manager.cpp"
