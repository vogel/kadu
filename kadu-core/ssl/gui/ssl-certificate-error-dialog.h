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

#include "ssl/ssl-certificate.h"

#include <QtCore/QPointer>
#include <QtNetwork/QSslError>
#include <QtWidgets/QDialog>

class SslCertificateRepository;

class KADUAPI SslCertificateErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SslCertificateErrorDialog(
        SslCertificate certificate, const QList<QSslError> &errors, QWidget *parent = nullptr);
    virtual ~SslCertificateErrorDialog();

    void setSslCertificateRepository(SslCertificateRepository *sslCertificateRepository);

private:
    QPointer<SslCertificateRepository> m_sslCertificateRepository;

    SslCertificate m_certificate;

    void createGui(const QList<QSslError> &errors);

private slots:
    void increaseHeight();
    void connectAnyway();
    void trustCertificate();
};
