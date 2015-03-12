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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <injeqt/injeqt.h>
#include <memory>

class Chat;
class ChatManager;
class StoragePoint;
class StoragePointFactory;

class SslCertificate;

class SslCertificateStorage : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SslCertificateStorage(QObject *parent = nullptr);
	virtual ~SslCertificateStorage();

	QSet<SslCertificate> loadCertificates() const;
	void storeCertificates(const QSet<SslCertificate> &certificates) const;

private:
	QPointer<StoragePointFactory> m_storagePointFactory;

	std::unique_ptr<StoragePoint> storagePoint() const;

private slots:
	INJEQT_SETTER void setStoragePointFactory(StoragePointFactory *storagePointFactory);

};
