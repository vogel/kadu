/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "qmessaging-menu-source.h"

#include "qmessaging-menu-app.h"

#include <messaging-menu/messaging-menu-app.h>

QMessagingMenuSource::QMessagingMenuSource(const QString &id, const QString &label, const QString &icon, QMessagingMenuApp *menuApp) :
	QObject{menuApp},
	m_menuApp{menuApp},
	m_id{id}
{
	Q_ASSERT(m_menuApp);
	Q_UNUSED(icon);

	messaging_menu_app_append_source(m_menuApp->app(), m_id.toUtf8().data(), nullptr, label.toUtf8().data());
}

QMessagingMenuSource::QMessagingMenuSource(const QString &id, const QString &label, const QString &icon, int count, QMessagingMenuApp *menuApp) :
	QObject{menuApp},
	m_menuApp{menuApp},
	m_id{id}
{
	Q_ASSERT(m_menuApp);
	Q_UNUSED(icon);

	auto gicon = !icon.isEmpty()
		? g_icon_new_for_string(icon.toUtf8().data(), nullptr)
		: nullptr;
	messaging_menu_app_append_source_with_count(m_menuApp->app(), m_id.toUtf8().data(), gicon, label.toUtf8().data(), count);
	if (gicon)
		g_object_unref(gicon);
}

QMessagingMenuSource::~QMessagingMenuSource()
{
	messaging_menu_app_remove_source(m_menuApp->app(), m_id.toUtf8().data());
}

void QMessagingMenuSource::setLabel(const QString &label)
{
	messaging_menu_app_set_source_label(m_menuApp->app(), m_id.toUtf8().data(), label.toUtf8().data());
}

void QMessagingMenuSource::setIcon(const QString &icon)
{
	auto gicon = !icon.isEmpty()
		? g_icon_new_for_string(icon.toUtf8().data(), nullptr)
		: nullptr;
	messaging_menu_app_set_source_icon(m_menuApp->app(), m_id.toUtf8().data(), gicon);
	if (gicon)
		g_object_unref(gicon);
}

void QMessagingMenuSource::setCount(int count)
{
	messaging_menu_app_set_source_count(m_menuApp->app(), m_id.toUtf8().data(), count);
}

void QMessagingMenuSource::drawAttention()
{
	messaging_menu_app_draw_attention(m_menuApp->app(), m_id.toUtf8().data());
}

void QMessagingMenuSource::removeAttention()
{
	messaging_menu_app_remove_attention(m_menuApp->app(), m_id.toUtf8().data());
}
