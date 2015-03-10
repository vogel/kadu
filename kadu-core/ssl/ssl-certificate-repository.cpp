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

#include "ssl-certificate-repository.h"

#include <QtNetwork/QSslCertificate>

QSslCertificate SslCertificateRepository::converter(SslCertificateRepository::WrappedIterator iterator)
{
	return QSslCertificate{*iterator, QSsl::EncodingFormat::Pem};
}

SslCertificateRepository::SslCertificateRepository(QObject *parent) :
		QObject{parent}
{
}

SslCertificateRepository::~SslCertificateRepository()
{
}

SslCertificateRepository::Iterator SslCertificateRepository::begin()
{
	return Iterator{m_certificates.begin(), converter};
}

SslCertificateRepository::Iterator SslCertificateRepository::end()
{
	return Iterator{m_certificates.end(), converter};
}

QVector<QSslCertificate> SslCertificateRepository::certificates() const
{
	auto result = QVector<QSslCertificate>{};
	for (auto &&certificate : m_certificates)
		result.append(QSslCertificate{certificate, QSsl::EncodingFormat::Pem});
	return result;
}

void SslCertificateRepository::setPersistentCertificates(const QVector<QSslCertificate> &certificates)
{
	m_persistentCertificates.clear();
	for (auto &&certificate : certificates)
		if (!certificate.isNull())
			m_persistentCertificates.insert(certificate.toPem());
	m_certificates = m_persistentCertificates;
}

QVector<QSslCertificate> SslCertificateRepository::persistentCertificates() const
{
	auto result = QVector<QSslCertificate>{};
	for (auto &&persistentCertificate : m_persistentCertificates)
		result.append(QSslCertificate{persistentCertificate, QSsl::EncodingFormat::Pem});
	return result;
}

bool SslCertificateRepository::containsCertificate(const QSslCertificate &certificate) const
{
	auto it = std::find(std::begin(m_certificates), std::end(m_certificates), certificate.toPem());
	return it != std::end(m_certificates);
}

void SslCertificateRepository::addCertificate(QSslCertificate certificate)
{
	m_certificates.insert(certificate.toPem());
}

void SslCertificateRepository::addPersistentCertificate(QSslCertificate certificate)
{
	m_certificates.insert(certificate.toPem());
	m_persistentCertificates.insert(certificate.toPem());
}

void SslCertificateRepository::removeCertificate(QSslCertificate certificate)
{
	auto it = std::find(std::begin(m_certificates), std::end(m_certificates), certificate.toPem());
	if (it != std::end(m_certificates))
		m_certificates.erase(it);

	auto persistentIt = std::find(std::begin(m_persistentCertificates), std::end(m_persistentCertificates), certificate.toPem());
	if (persistentIt != std::end(m_persistentCertificates))
		m_persistentCertificates.erase(persistentIt);
}

#include "moc_ssl-certificate-repository.cpp"
