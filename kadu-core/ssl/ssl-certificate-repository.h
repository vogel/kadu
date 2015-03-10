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

#include "misc/iterator.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QVector>
#include <injeqt/injeqt.h>

class QByteArray;
class QSslCertificate;

class KADUAPI SslCertificateRepository : public QObject
{
	Q_OBJECT

	using Storage = QSet<QByteArray>;
	using WrappedIterator = Storage::iterator;

public:
	using Iterator = IteratorWrapper<WrappedIterator, QSslCertificate>;

	Q_INVOKABLE explicit SslCertificateRepository(QObject *parent = nullptr);
	virtual ~SslCertificateRepository();

	Iterator begin();
	Iterator end();

	QVector<QSslCertificate> certificates() const;

	void setPersistentCertificates(const QVector<QSslCertificate> &certificates);
	QVector<QSslCertificate> persistentCertificates() const;

	bool containsCertificate(const QSslCertificate &certificate) const;

public slots:
	void addCertificate(QSslCertificate certificate);
	void addPersistentCertificate(QSslCertificate certificate);
	void removeCertificate(QSslCertificate certificate);

private:
	static QSslCertificate converter(WrappedIterator iterator);

	Storage m_certificates;
	Storage m_persistentCertificates;

};

inline SslCertificateRepository::Iterator begin(SslCertificateRepository *sslCertificateRepository)
{
	return sslCertificateRepository->begin();
}

inline SslCertificateRepository::Iterator end(SslCertificateRepository *sslCertificateRepository)
{
	return sslCertificateRepository->end();
}

/**
 * @}
 */
