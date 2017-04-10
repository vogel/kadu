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

#pragma once

#include "contacts/contact.h"

#include <QtCore/QPointer>
#include <QtWidgets/QLabel>
#include <injeqt/injeqt.h>

class Avatars;
class IconsManager;
struct AvatarId;

class ContactAvatarDisplay : public QLabel
{
    Q_OBJECT

public:
    explicit ContactAvatarDisplay(Contact contact, QSize size, QWidget *parent = nullptr);
    virtual ~ContactAvatarDisplay();

private:
    QPointer<Avatars> m_avatars;
    QPointer<IconsManager> m_iconsManager;

    Contact m_contact;
    QSize m_size;

    void displayAvatar();

private slots:
    INJEQT_SET void setAvatars(Avatars *avatars);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_INIT void init();

    void avatarUpdated(const AvatarId &id);
};
