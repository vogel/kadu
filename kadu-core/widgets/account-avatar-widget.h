/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "exports.h"
#include "misc/memory.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QLabel;
class QPushButton;

class AggregatedAccountAvatarService;
class Avatars;
class IconsManager;
struct AvatarId;

class KADUAPI AccountAvatarWidget : public QWidget
{
    Q_OBJECT

    enum
    {
        ModeChange,
        ModeRemove
    } Mode;

public:
    explicit AccountAvatarWidget(Account account, QWidget *parent = nullptr);
    virtual ~AccountAvatarWidget() = default;

private:
    QPointer<AggregatedAccountAvatarService> m_aggregatedAccountAvatarService;
    QPointer<Avatars> m_avatars;
    QPointer<IconsManager> m_iconsManager;

    Account m_account;
    QPixmap m_avatar;

    owned_qptr<QLabel> m_avatarLabel;
    owned_qptr<QMovie> m_waitMovie;
    owned_qptr<QPushButton> m_changePhotoButton;

    void createGui();
    void setupMode();

    void uploadAvatar();
    void changeAvatar();
    void removeAvatar();

    void serviceAvailabilityChanged();

private slots:
    INJEQT_SET void setAggregatedAccountAvatarService(AggregatedAccountAvatarService *aggregatedAccountAvatarService);
    INJEQT_SET void setAvatars(Avatars *avatars);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_INIT void init();

    void avatarUpdated(const AvatarId &id);
    void uploadFinished(const Account &account, bool ok);

    void changeButtonClicked();
};
