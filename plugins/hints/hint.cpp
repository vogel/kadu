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

#include "chat/chat.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "icons/icons-manager.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-configuration.h"
#include "notification/notification-service.h"
#include "notification/notification.h"
#include "debug.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

Hint::Hint(QWidget *parent, const Notification &xnotification)
	: QFrame(parent), vbox(0), callbacksBox(0), icon(0), label(0), notification(xnotification)
{
}

Hint::~Hint()
{
}

void Hint::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
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
	auto key = m_notificationConfiguration->notifyConfigurationKey(notification.type);
	startSecs = secs = m_configuration->deprecatedApi()->readNumEntry("Hints", "Event_" + key + "_timeout", 10);

	createLabels(m_iconsManager->iconByPath(notification.icon).pixmap(m_configuration->deprecatedApi()->readNumEntry("Hints", "AllEvents_iconSize", 32)));

	auto showButtons = !notification.callbacks.isEmpty() && m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowNotificationActions", 
			!m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowOnlyNecessaryButtons", false));
	if (showButtons)
	{
		callbacksBox = new QHBoxLayout();
		callbacksBox->addStretch(10);
		vbox->addLayout(callbacksBox);

		for (auto &&callbackName : notification.callbacks)
		{
			auto callback = m_notificationCallbackRepository->callback(callbackName);
			auto button = new QPushButton(callback.title(), this);
			button->setProperty("notify:callback", callbackName);
			connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

			callbacksBox->addWidget(button);
			callbacksBox->addStretch(1);
		}

		callbacksBox->addStretch(9);
	}

	configurationUpdated();
	updateText();
	show();

	kdebugf2();
}

void Hint::buttonClicked()
{
	auto callbackNotification = notification;

	auto callbackName = sender()->property("notify:callback").toString();
	if (!callbackName.isEmpty())
	{
		auto callback = m_notificationCallbackRepository->callback(callbackName);
		callback.call(callbackNotification);
	}

	close();
}

void Hint::configurationUpdated()
{
	QFont font(qApp->font());
	QPalette palette(qApp->palette());

	auto key = m_notificationConfiguration->notifyConfigurationKey(notification.type);
	label->setFont(m_configuration->deprecatedApi()->readFontEntry("Hints", "Event_" + key + "_font", &font));

	updateText();
}

void Hint::createLabels(const QPixmap &pixmap)
{
	vbox = new QVBoxLayout(this);
	vbox->setSpacing(0);
	labels = new QHBoxLayout();
	labels->setSpacing(0);
	labels->setContentsMargins(6, 4, 6, 4);
	vbox->addLayout(labels);

	if (!pixmap.isNull())
	{
		icon = new QLabel(this);
		icon->setPixmap(pixmap);
		icon->setContentsMargins(0, 0, 6, 0);
		icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		// Without setting fixed size right margin is sometimes not respected on Windows.
		// If you cannot reproduce, try setting MarginSize to 5 px.
		icon->setFixedSize(icon->sizeHint());
		labels->addWidget(icon, 0, Qt::AlignTop);
	}

	label = new QLabel(this);
	label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
	label->setTextInteractionFlags(Qt::NoTextInteraction);
	label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	label->setWordWrap(true);
	labels->addWidget(label);
}

void Hint::updateText()
{
	auto text = notification.text;

	if (m_configuration->deprecatedApi()->readBoolEntry("Hints", "ShowContentMessage"))
	{
		auto citeSign = m_configuration->deprecatedApi()->readNumEntry("Hints", "CiteSign");
		auto syntax = QStringLiteral("\n <small>%1</small>");
		auto message = notification.details.replace("<br/>", QStringLiteral(""));
		if (message.length() > citeSign)
			text += syntax.arg(message.left(citeSign) + "...");
		else
			text += syntax.arg(message);
	}

	text = text.replace('\n', QStringLiteral("<br />"));

	label->setText(QStringLiteral("<div style='width:100%; height:100%; vertical-align:middle;'>%1</div>").arg(text));

	adjustSize();
	updateGeometry();
	emit updated(this);
}

void Hint::resetTimeout()
{
	secs = startSecs;
}

void Hint::nextSecond(void)
{
	if (startSecs == 0)
		return;

	if (secs == 0)
	{
		kdebugm(KDEBUG_ERROR, "ERROR: secs == 0 !\n");
	}
	else if (secs > 2000000000)
	{
		kdebugm(KDEBUG_WARNING, "WARNING: secs > 2 000 000 000 !\n");
	}

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
	m_notificationService->acceptNotification(notification);
}

void Hint::discardNotification()
{
	m_notificationService->discardNotification(notification);
}

#include "moc_hint.cpp"
