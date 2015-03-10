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

#include "ssl-certificate-storage.h"

#include "chat/chat-manager.h"
#include "storage/chat-list-storage.h"
#include "storage/storage-point-factory.h"
#include "storage/string-list-storage.h"

#include <QtNetwork/QSslCertificate>

SslCertificateStorage::SslCertificateStorage(QObject *parent) :
		QObject{parent}
{
}

SslCertificateStorage::~SslCertificateStorage()
{
}

void SslCertificateStorage::setStoragePointFactory(StoragePointFactory *storagePointFactory)
{
	m_storagePointFactory = storagePointFactory;
}

std::unique_ptr<StoragePoint> SslCertificateStorage::storagePoint() const
{
	if (!m_storagePointFactory)
		return {};
	return m_storagePointFactory.data()->createStoragePoint(QLatin1String("SslCertificates"));
}

QVector<QSslCertificate> SslCertificateStorage::loadCertificates() const
{
	auto storage = storagePoint();
	if (!storage)
		return {};

	auto stringListStorage = StringListStorage{storage.get(), QLatin1String{"Certificate"}};
	return certificatesFromStringList(stringListStorage.load());
}

QVector<QSslCertificate> SslCertificateStorage::certificatesFromStringList(const QStringList& strings) const
{
	auto result = QVector<QSslCertificate>{};
	for (auto &&string : strings)
	{
		auto certificate = QSslCertificate{QByteArray::fromHex(string.toLatin1()), QSsl::EncodingFormat::Pem};
		if (!certificate.isNull())
			result.append(certificate);
	}
	return result;
}

void SslCertificateStorage::storeCertificates(const QVector<QSslCertificate> &certificates) const
{
	auto storage = storagePoint();
	if (!storage)
		return;

	auto stringListStorage = StringListStorage{storage.get(), QLatin1String{"Certificate"}};
	stringListStorage.store(certificatesToStringList(certificates));
}

QStringList SslCertificateStorage::certificatesToStringList(const QVector<QSslCertificate> &certificates) const
{
	auto result = QStringList{};
	for (auto &&certificate : certificates)
		if (!certificate.isNull())
			result.append(QString::fromLatin1(certificate.toPem().toHex()));
	return result;
}

#include "moc_ssl-certificate-storage.cpp"
