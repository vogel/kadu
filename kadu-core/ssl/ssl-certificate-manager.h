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

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <functional>
#include <injeqt/injeqt.h>

class SslCertificateStorage;
class SslCertificateRepository;

class QSslCertificate;
class QSslError;

class KADUAPI SslCertificateManager : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SslCertificateManager(QObject *parent = nullptr);
    virtual ~SslCertificateManager();

    void loadPersistentSslCertificates();
    void storePersistentSslCertificates();

    bool acceptCertificate(const QString &hostName, const QSslError &error) const;
    void askForCertificateAcceptance(
        const QString &hostName, const QSslCertificate &certificate, const QList<QSslError> &errors,
        const std::function<void()> &onAccepted, const std::function<void()> &onRejected);

private:
    QPointer<SslCertificateRepository> m_sslCertificateRepository;
    QPointer<SslCertificateStorage> m_sslCertificateStorage;

private slots:
    INJEQT_SET void setSslCertificateRepository(SslCertificateRepository *sslCertificateRepository);
    INJEQT_SET void setSslCertificateStorage(SslCertificateStorage *sslCertificateStorage);
};
