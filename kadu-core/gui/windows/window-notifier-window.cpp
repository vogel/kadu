/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "window-notifier-window.h"

#include "icons/icons-manager.h"
#include "notification/notification/aggregate-notification.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification/notification.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

WindowNotifierWindow::WindowNotifierWindow(Notification *notification, QWidget *parent) :
		QDialog{parent, Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint},
		DesktopAwareObject{this},
		m_notification{notification}
{
	setWindowRole("kadu-window-notifier");

	setWindowTitle(m_notification->title());
	setAttribute(Qt::WA_DeleteOnClose);
}

WindowNotifierWindow::~WindowNotifierWindow()
{
	emit closed(m_notification);
}

void WindowNotifierWindow::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
	createGui();
}

void WindowNotifierWindow::createGui()
{
	auto layout = new QVBoxLayout{this};
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setSpacing(10);

	auto labels = new QWidget{};
	auto labelsLayout = new QHBoxLayout{labels};
	labelsLayout->setSpacing(10);

	if (!m_notification->icon().icon().isNull())
	{
		auto iconLabel = new QLabel{};
		iconLabel->setPixmap(m_notification->icon().icon().pixmap(64, 64));
		labelsLayout->addWidget(iconLabel);
	}

	auto textLabel = new QLabel{};
	auto text = m_notification->text();
	if (!m_notification->details().isEmpty())
		text += "<br/> <small>" + m_notification->details().join("<br/>") + "</small>";
	textLabel->setText(text);

	labelsLayout->addWidget(textLabel);

	layout->addWidget(labels, 0, Qt::AlignCenter);

	auto buttons = new QWidget{};
	auto buttonsLayout = new QHBoxLayout{buttons};
	buttonsLayout->setSpacing(20);

	layout->addWidget(buttons, 0, Qt::AlignCenter);

	auto callbacks = m_notification->getCallbacks();
	if (!callbacks.isEmpty())
		for (auto &&callbackName : callbacks)
		{
			auto callback = m_notificationCallbackRepository->callback(callbackName);
			addButton(buttons, callback.title(), callbackName);
		}
	else
		addButton(buttons, tr("OK"), QString{});

	connect(m_notification, SIGNAL(closed(Notification *)), this, SLOT(close()));

	buttons->setMaximumSize(buttons->sizeHint());
}

void WindowNotifierWindow::addButton(QWidget *parent, const QString &title, const QString &name)
{
	auto button = new QPushButton{};
	parent->layout()->addWidget(button);

	button->setText(title);
	button->setProperty("notify:callback", name);
	connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void WindowNotifierWindow::buttonClicked()
{
	auto callbackNotifiation = m_notification;
	if (qobject_cast<AggregateNotification *>(callbackNotifiation))
		callbackNotifiation = qobject_cast<AggregateNotification *>(callbackNotifiation)->notifications()[0];

	auto callbackName = sender()->property("notify:callback").toString();
	if (!callbackName.isEmpty())
	{
		auto callback = m_notificationCallbackRepository->callback(callbackName);
		callback.call(callbackNotifiation);
	}

	m_notification->close();
	close();
}

#include "moc_window-notifier-window.cpp"
