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

#include "contact-avatar-display.h"
#include "contact-avatar-display.moc"

#include "avatars/avatar-id.h"
#include "avatars/avatars.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"

#include <QtGui/QIcon>

ContactAvatarDisplay::ContactAvatarDisplay(Contact contact, QSize size, QWidget *parent)
        : QLabel{parent}, m_contact{contact}, m_size{size}
{
    setFixedWidth(m_size.width());
}

ContactAvatarDisplay::~ContactAvatarDisplay()
{
}

void ContactAvatarDisplay::setAvatars(Avatars *avatars)
{
    m_avatars = avatars;
}

void ContactAvatarDisplay::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void ContactAvatarDisplay::init()
{
    connect(m_avatars, &Avatars::updated, this, &ContactAvatarDisplay::avatarUpdated);
    displayAvatar();
}

void ContactAvatarDisplay::avatarUpdated(const AvatarId &id)
{
    if (id == avatarId(m_contact) || id == avatarId(m_contact.ownerBuddy()))
        displayAvatar();
}

void ContactAvatarDisplay::displayAvatar()
{
    auto pixmap = m_avatars->pixmap(avatarIds(m_contact));
    if (pixmap.isNull())
        pixmap = m_iconsManager->iconByPath(KaduIcon{"kadu_icons/buddy0"}).pixmap(m_size);
    if (!pixmap.isNull())
        pixmap = pixmap.scaled(m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    setPixmap(pixmap);
}
