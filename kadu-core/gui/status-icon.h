/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-aware-object.h"
#include "exports.h"
#include "icons/kadu-icon.h"
#include "status/status.h"

#include <QtCore/QPointer>
#include <QtWidgets/QPushButton>
#include <injeqt/injeqt.h>

class StatusContainer;

class KADUAPI StatusIcon : public QObject, private ConfigurationAwareObject
{
    Q_OBJECT

    StatusContainer *MyStatusContainer;
    QTimer *BlinkTimer;
    bool BlinkOffline;

    KaduIcon Icon;

    void updateStatus();

    void enableBlink();
    void disableBlink();

    void setIcon(const KaduIcon &icon);

private slots:
    INJEQT_INIT void init();

    void blink();
    void statusUpdated(StatusContainer *container = 0);

protected:
    virtual void configurationUpdated();

public:
    explicit StatusIcon(StatusContainer *statusContainer, QObject *parent = nullptr);
    virtual ~StatusIcon();

    const KaduIcon &icon() const
    {
        return Icon;
    }

signals:
    void iconUpdated(const KaduIcon &icon);
};
