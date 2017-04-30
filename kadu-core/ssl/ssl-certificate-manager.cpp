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
#include "ssl-certificate-manager.moc"

#include "ssl/gui/ssl-certificate-error-dialog.h"
#include "ssl/ssl-certificate-repository.h"
#include "ssl/ssl-certificate-storage.h"
#include "ssl/ssl-certificate.h"

#include <QtNetwork/QSslCertificate>

SslCertificateManager::SslCertificateManager(QObject *parent) : QObject(parent)
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

bool SslCertificateManager::acceptCertificate(const QString &hostName, const QSslError &error) const
{
    if (error.error() == QSslError::HostNameMismatch)
        if (m_sslCertificateRepository->containsCertificateFor(hostName, error.certificate().subjectAlternativeNames().values(QSsl::DnsEntry)))
            return true;

    auto hostCertificate = SslCertificate{hostName, error.certificate().toPem().toHex()};
    return m_sslCertificateRepository->containsCertificate(hostCertificate);
}

void SslCertificateManager::askForCertificateAcceptance(
    const QString &hostName, const QSslCertificate &certificate, const QList<QSslError> &errors,
    const std::function<void()> &onAccepted, const std::function<void()> &onRejected)
{
    auto hostCertificate = SslCertificate{hostName, certificate.toPem().toHex()};
    auto sslCertificateErrorDialog = new SslCertificateErrorDialog{hostCertificate, errors};
    sslCertificateErrorDialog->setSslCertificateRepository(m_sslCertificateRepository);
    connect(sslCertificateErrorDialog, &SslCertificateErrorDialog::accepted, onAccepted);
    connect(sslCertificateErrorDialog, &SslCertificateErrorDialog::rejected, onRejected);

    sslCertificateErrorDialog->show();
}
