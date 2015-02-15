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

#include "avatars/avatar.h"
#include "icons/kadu-icon.h"

#include <QtGui/QIcon>

ContactAvatarDisplay::ContactAvatarDisplay(Contact contact, QSize size, QWidget *parent) :
		QLabel{parent},
		m_contact{contact},
		m_avatar{m_contact.avatar(true)},
		m_size{size}
{
	setFixedWidth(m_size.width());

	connect(m_contact, SIGNAL(updated()), this, SLOT(avatarUpdated()));
	connect(m_avatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));

	displayAvatar();
}

ContactAvatarDisplay::~ContactAvatarDisplay()
{
}

void ContactAvatarDisplay::avatarUpdated()
{
	disconnect(m_avatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));
	m_avatar = m_contact.avatar(true);
	connect(m_avatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));

	displayAvatar();
}

void ContactAvatarDisplay::displayAvatar()
{
	auto pixmap = m_avatar.pixmap();
	if (pixmap.isNull())
		pixmap = KaduIcon{"kadu_icons/buddy0"}.icon().pixmap(m_size);
	if (!pixmap.isNull())
		pixmap = pixmap.scaled(m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	setPixmap(pixmap);
}

#include "moc_contact-avatar-display.cpp"
