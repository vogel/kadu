/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "avatar-job-manager.h"
#include "avatar-job-manager.moc"

#include "avatars/avatar-job-runner.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"

#include <QtCore/QTimer>

AvatarJobManager::AvatarJobManager(QObject *parent) : QObject{parent}
{
    m_timer.setInterval(500);
    connect(&m_timer, &QTimer::timeout, this, &AvatarJobManager::runJob);
}

AvatarJobManager::~AvatarJobManager()
{
}

void AvatarJobManager::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void AvatarJobManager::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void AvatarJobManager::scheduleJob()
{
    if (!m_timer.isActive())
        m_timer.start();
}

void AvatarJobManager::runJob()
{
    if (m_jobs.isEmpty())
    {
        m_timer.stop();
        return;
    }

    if (!m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowAvatars", true))
        return;

    m_injectedFactory->makeInjected<AvatarJobRunner>(nextJob(), this)->runJob();
}

void AvatarJobManager::addJob(const Contact &contact)
{
    if (!contact)
        return;

    m_jobs.insert(contact);
    scheduleJob();
}

Contact AvatarJobManager::nextJob()
{
    auto job = *m_jobs.constBegin();
    m_jobs.remove(job);

    return job;
}
