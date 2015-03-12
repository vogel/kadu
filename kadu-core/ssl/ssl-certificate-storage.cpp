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
#include "ssl/ssl-certificate.h"
#include "storage/chat-list-storage.h"
#include "storage/storage-point-factory.h"

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

QSet<SslCertificate> SslCertificateStorage::loadCertificates() const
{
	auto storage = storagePoint();
	if (!storage)
		return {};

	auto result = QSet<SslCertificate>{};
	auto elements = storage->storage()->getNodes(storage->point(), QLatin1String{"Certificate"});
	for (const auto &element : elements)
	{
		auto hostName = element.attribute("hostName");
		auto pemHexEncodedCertificate = element.text();
		if (!hostName.isEmpty() && !pemHexEncodedCertificate.isEmpty())
			result.insert(SslCertificate{hostName, pemHexEncodedCertificate});
	}
	return result;
}

void SslCertificateStorage::storeCertificates(const QSet<SslCertificate> &certificates) const
{
	auto storage = storagePoint();
	if (!storage)
		return;

	storage->storage()->removeChildren(storage->point());

	for (const auto &certificate : certificates)
	{
		auto element = storage->storage()->createElement(storage->point(), QLatin1String{"Certificate"});
		auto textNode = element.ownerDocument().createTextNode(certificate.pemHexEncodedCertificate());
		element.setAttribute("hostName", certificate.hostName());
		element.appendChild(textNode);
	}
}

#include "moc_ssl-certificate-storage.cpp"
