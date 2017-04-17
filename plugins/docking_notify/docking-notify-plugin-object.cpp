/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking-notify-plugin-object.h"
#include "docking-notify-plugin-object.moc"

#include "docking-notifier.h"

#include "notification/notifier-repository.h"

DockingNotifyPluginObject::DockingNotifyPluginObject(QObject *parent) : QObject{parent}
{
}

DockingNotifyPluginObject::~DockingNotifyPluginObject()
{
}

void DockingNotifyPluginObject::setDockingNotifier(DockingNotifier *dockingNotifier)
{
    m_dockingNotifier = dockingNotifier;
}

void DockingNotifyPluginObject::setNotifierRepository(NotifierRepository *notifierRepository)
{
    m_notifierRepository = notifierRepository;
}

void DockingNotifyPluginObject::init()
{
    m_notifierRepository->registerNotifier(m_dockingNotifier);
}

void DockingNotifyPluginObject::done()
{
    m_notifierRepository->unregisterNotifier(m_dockingNotifier);
}
