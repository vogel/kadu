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

#include "qunity.h"

#include <unity.h>

QUnity::QUnity(const QString &desktopId, QObject* parent) :
	QObject{parent},
	m_launcher{nullptr}
{
	m_launcher = unity_launcher_entry_get_for_desktop_id(desktopId.toUtf8().data());
	g_object_ref(m_launcher);
}

QUnity::~QUnity()
{
	unity_launcher_entry_set_count_visible(m_launcher, false);
	g_object_unref(m_launcher);
	m_launcher = nullptr;
}

void QUnity::updateCount(int count)
{
	if (count == 0)
	{
		unity_launcher_entry_set_count_visible(m_launcher, false);
	}
	else
	{
		unity_launcher_entry_set_count_visible(m_launcher, true);
		unity_launcher_entry_set_count(m_launcher, count);
	}
}

void QUnity::updateProgress(int progress)
{
	if (progress < 100)
	{
		unity_launcher_entry_set_progress_visible(m_launcher, true);
		unity_launcher_entry_set_progress(m_launcher, static_cast<double>(progress) / 100.0);
	}
	else
		unity_launcher_entry_set_progress_visible(m_launcher, false);
}

#include "moc_qunity.cpp"
