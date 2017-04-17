/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "hint.h"
#include "hint.moc"

#include "hints-configuration.h"

#include "html/html-conversion.h"
#include "icons/icons-manager.h"
#include "misc/memory.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-configuration.h"
#include "notification/notification-service.h"
#include "notification/notification.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

Hint::Hint(const Notification &notification, HintsConfiguration *hintsConfiguration, QWidget *parent)
        : QFrame{parent}, m_hintsConfiguration{hintsConfiguration}, m_notification{notification}
{
}

Hint::~Hint()
{
    emit hintDestroyed(this);
}

void Hint::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void Hint::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
    m_notificationCallbackRepository = notificationCallbackRepository;
}

void Hint::setNotificationConfiguration(NotificationConfiguration *notificationConfiguration)
{
    m_notificationConfiguration = notificationConfiguration;
}

void Hint::setNotificationService(NotificationService *notificationService)
{
    m_notificationService = notificationService;
}

void Hint::init()
{
    startSecs = secs = 10;

    createGui();
}

void Hint::createGui()
{
    auto pixmap = m_iconsManager->iconByPath(m_notification.icon).pixmap(m_hintsConfiguration->iconSize());
    auto detailsText = details();
    auto withDetailsText = !detailsText.isEmpty();

    auto icon = make_owned<QLabel>(this);
    icon->setPixmap(pixmap);
    icon->setContentsMargins(0, 0, 6, 0);
    icon->setFixedSize(icon->sizeHint());

    auto label = make_owned<QLabel>(this);
    label->setTextInteractionFlags(Qt::NoTextInteraction);
    label->setText(m_notification.text.string());

    auto layout = make_owned<QGridLayout>(this);
    layout->setSpacing(0);
    layout->addWidget(icon, 0, 0, 2, 1, Qt::AlignTop);
    layout->addWidget(label, 0, 1, withDetailsText ? 1 : 2, 1, Qt::AlignVCenter | Qt::AlignLeft);

    if (withDetailsText)
    {
        auto detailsLabel = make_owned<QLabel>(this);
        auto font = detailsLabel->font();
        font.setPointSizeF(font.pointSizeF() * 0.8);
        detailsLabel->setFont(font);
        detailsLabel->setTextInteractionFlags(Qt::NoTextInteraction);
        detailsLabel->setText(detailsText.replace('\n', QStringLiteral("<br />")));

        layout->addWidget(detailsLabel, 1, 1, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (shouldShowButtons())
    {
        auto f = font();
        f.setPointSizeF(f.pointSizeF() * 0.8);

        auto callbacksBox = new QHBoxLayout{};
        layout->addLayout(callbacksBox, 2, 0, 1, 2, Qt::AlignRight);

        for (auto &&callbackName : m_notification.callbacks)
        {
            auto callback = m_notificationCallbackRepository->callback(callbackName);
            auto button = make_owned<QToolButton>(this);
            button->setFont(f);
            button->setText(callback.title());
            button->setToolButtonStyle(Qt::ToolButtonTextOnly);
            button->setProperty("notify:callback", callbackName);
            connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

            callbacksBox->addWidget(button);
        }
    }
}

QString Hint::details() const
{
    if (!m_hintsConfiguration->showContentMessage() || m_notification.details.string().isEmpty())
        return {};

    auto const citeSign = 50;
    auto const message = htmlToPlain(m_notification.details);
    return message.length() > citeSign ? message.left(citeSign) + "..." : message;
}

bool Hint::shouldShowButtons() const
{
    if (m_notification.callbacks.isEmpty())
        return false;

    if (m_hintsConfiguration->showAllNotificationActions())
        return true;

    if (m_notification.callbacks.size() != 2)
        return true;

    auto acceptCallback =
        m_notification.acceptCallback.isEmpty() ? QStringLiteral("chat-open") : m_notification.acceptCallback;
    auto discardCallback =
        m_notification.discardCallback.isEmpty() ? QStringLiteral("ignore") : m_notification.discardCallback;

    return m_notification.callbacks[0] != acceptCallback || m_notification.callbacks[1] != discardCallback;
}

void Hint::buttonClicked()
{
    auto const callbackName = sender()->property("notify:callback").toString();
    if (!callbackName.isEmpty())
    {
        auto callback = m_notificationCallbackRepository->callback(callbackName);
        callback.call(m_notification);
    }

    deleteLater();
}

void Hint::nextSecond(void)
{
    if (startSecs == 0)
        return;

    if (secs > 0)
        --secs;
}

bool Hint::isDeprecated()
{
    return startSecs != 0 && secs == 0;
}

void Hint::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        emit leftButtonClicked(this);
        break;

    case Qt::RightButton:
        emit rightButtonClicked(this);
        break;

    case Qt::MidButton:
        emit midButtonClicked(this);
        break;

    default:
        break;
    }
}

void Hint::enterEvent(QEvent *)
{
    setStyleSheet(QStringLiteral("* {background-color:%1;}").arg(palette().window().color().lighter().name()));
}

void Hint::leaveEvent(QEvent *)
{
    setStyleSheet(QStringLiteral(""));
}

void Hint::acceptNotification()
{
    m_notificationService->acceptNotification(m_notification);
}

void Hint::discardNotification()
{
    m_notificationService->discardNotification(m_notification);
}
