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

#include "notification/notification.h"

#include <QtCore/QPointer>
#include <QtWidgets/QFrame>
#include <injeqt/injeqt.h>

class Chat;
class HintsConfiguration;
class IconsManager;
class NotificationCallbackRepository;
class NotificationConfiguration;
class NotificationService;
struct Notification;

class Hint : public QFrame
{
    Q_OBJECT

public:
    explicit Hint(const Notification &notification, HintsConfiguration *hintsConfiguration, QWidget *parent = nullptr);
    virtual ~Hint();

    bool isDeprecated();

public slots:
    void nextSecond();

    void acceptNotification();
    void discardNotification();

signals:
    void leftButtonClicked(Hint *hint);
    void rightButtonClicked(Hint *hint);
    void midButtonClicked(Hint *hint);
    void hintDestroyed(Hint *hint);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    QPointer<HintsConfiguration> m_hintsConfiguration;
    QPointer<IconsManager> m_iconsManager;
    QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;
    QPointer<NotificationConfiguration> m_notificationConfiguration;
    QPointer<NotificationService> m_notificationService;

    int secs;
    int startSecs;

    Notification m_notification;

    void createGui();
    QString details() const;
    bool shouldShowButtons() const;

private slots:
    INJEQT_SET void setIconsManager(IconsManager *IconsManager);
    INJEQT_SET void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);
    INJEQT_SET void setNotificationConfiguration(NotificationConfiguration *notificationConfiguration);
    INJEQT_SET void setNotificationService(NotificationService *notificationService);
    INJEQT_INIT void init();

    void buttonClicked();
};
