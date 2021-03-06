/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "sms-sender.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;

class QProcess;

class SmsExternalSender : public SmsSender
{
    Q_OBJECT

    QPointer<Configuration> m_configuration;
    QProcess *Process;

    QStringList buildProgramArguments(const QString &message);

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);

    void processFinished();

public:
    explicit SmsExternalSender(const QString &number, QObject *parent = nullptr);
    virtual ~SmsExternalSender();

    virtual void sendMessage(const QString &message);

public slots:
    virtual void cancel();
};
