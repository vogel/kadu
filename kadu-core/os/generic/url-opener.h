/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class IconsManager;

class KADUAPI UrlOpener : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit UrlOpener(QObject *parent = nullptr);
    virtual ~UrlOpener();

    bool
    openUrl(const QByteArray &urlForDesktopServices, const QByteArray &urlForApplication, const QString &application);
    void openUrl(const QByteArray &url);
    void openEmail(const QByteArray &email);

private:
    QPointer<Configuration> m_configuration;
    QPointer<IconsManager> m_iconsManager;

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
};
