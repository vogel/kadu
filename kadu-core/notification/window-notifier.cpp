/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "window-notifier.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "gui/windows/window-notifier-window.h"
#include "icons/icons-manager.h"
#include "notification/notification-manager.h"
#include "notification/notification/notification.h"
#include "activate.h"

/**
 * @ingroup window_notify
 * @{
 */

WindowNotifier::WindowNotifier(QObject *parent) :
		QObject{parent},
		Notifier{"Window", QT_TRANSLATE_NOOP("@default", "Show a window with notification"), KaduIcon("dialog-information")}
{
}

WindowNotifier::~WindowNotifier()
{
}

void WindowNotifier::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void WindowNotifier::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void WindowNotifier::init()
{
	createDefaultConfiguration();
}

void WindowNotifier::notify(Notification *notification)
{
	notification->acquire(this);

	auto window = m_injectedFactory->makeInjected<WindowNotifierWindow>(notification);

	connect(window, SIGNAL(closed(Notification *)), this, SLOT(notificationClosed(Notification *)));
	window->show();
	_activateWindow(window);
}

void WindowNotifier::notificationClosed(Notification *notification)
{
	notification->release(this);
}

void WindowNotifier::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer/IncomingFile_Window", true);
}

/** @} */

#include "moc_window-notifier.cpp"
