/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define QT_NO_SIGNALS_SLOTS_KEYWORDS

#include "qmessaging-menu-app.h"

#include "qmessaging-menu-source.h"
#include "qmessaging-menu-status.h"

#include <messaging-menu/messaging-menu-app.h>

static void source_activated(MessagingMenuApp *app, const gchar *id, gpointer user_data);
static void status_changed(MessagingMenuApp *app, MessagingMenuStatus status, gpointer user_data);

QMessagingMenuApp::QMessagingMenuApp(const QString &desktopId, QObject* parent) :
	QObject{parent},
	m_app{nullptr}
{
	m_app = messaging_menu_app_new(desktopId.toAscii());
	g_object_ref(m_app);

	g_signal_connect(m_app, "activate-source", G_CALLBACK(source_activated), this);
	g_signal_connect(m_app, "status-changed", G_CALLBACK(status_changed), this);
}

QMessagingMenuApp::~QMessagingMenuApp()
{
	g_object_unref(m_app);
	m_app = nullptr;
}

void QMessagingMenuApp::registerMenu()
{
	messaging_menu_app_register(m_app);
}

void QMessagingMenuApp::unregisterMenu()
{
	messaging_menu_app_unregister(m_app);
}

QMessagingMenuSource * QMessagingMenuApp::addSource(const QString &id, const QString &label, const QString &icon, int count)
{
	auto existing = source(id);
	if (existing)
	{
		existing->setLabel(label);
		existing->setIcon(icon);
		existing->setCount(count);
		return existing;
	}

	auto new_source = new QMessagingMenuSource(id, label, icon, count, this);
	m_sources.insert(id, new_source);
	return new_source;
}

void QMessagingMenuApp::removeSource(const QString &id)
{
	auto old_source = source(id);
	if (old_source)
	{
		m_sources.remove(id);
		delete old_source;
	}
}

QMessagingMenuSource * QMessagingMenuApp::source(const QString &id)
{
	return m_sources.value(id);
}

void QMessagingMenuApp::setStatus(QMessagingMenuStatus status)
{
	messaging_menu_app_set_status(m_app, static_cast<MessagingMenuStatus>(status));
}

MessagingMenuApp * QMessagingMenuApp::app() const
{
	return m_app;
}

void QMessagingMenuApp::sourceActivatedSlot(const QString &id)
{
	emit sourceActivated(id);
}

void QMessagingMenuApp::statusChangedSlot(QMessagingMenuStatus status)
{
	emit statusChanged(status);
}

static void source_activated(MessagingMenuApp *app, const gchar *id, gpointer user_data)
{
	Q_UNUSED(app);

	auto messagingMenuApp = static_cast<QMessagingMenuApp *>(user_data);
	messagingMenuApp->sourceActivatedSlot(QString::fromUtf8(id));
}

void status_changed(MessagingMenuApp *app, MessagingMenuStatus status, gpointer user_data)
{
	Q_UNUSED(app);

	auto messagingMenuApp = static_cast<QMessagingMenuApp *>(user_data);
	messagingMenuApp->statusChangedSlot(static_cast<QMessagingMenuStatus>(status));
}

#include "moc_qmessaging-menu-app.cpp"
