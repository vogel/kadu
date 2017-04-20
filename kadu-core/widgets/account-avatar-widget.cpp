/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "account-avatar-widget.h"
#include "account-avatar-widget.moc"

#include "avatars/aggregated-account-avatar-service.h"
#include "avatars/avatar-id.h"
#include "avatars/avatars.h"
#include "contacts/contact.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"

#include <QtGui/QMovie>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

AccountAvatarWidget::AccountAvatarWidget(Account account, QWidget *parent) : QWidget{parent}, m_account{account}
{
}

void AccountAvatarWidget::setAggregatedAccountAvatarService(
    AggregatedAccountAvatarService *aggregatedAccountAvatarService)
{
    m_aggregatedAccountAvatarService = aggregatedAccountAvatarService;
}

void AccountAvatarWidget::setAvatars(Avatars *avatars)
{
    m_avatars = avatars;
}

void AccountAvatarWidget::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void AccountAvatarWidget::init()
{
    m_waitMovie =
        make_owned<QMovie>(m_iconsManager->iconPath(KaduIcon("kadu_icons/please-wait", "16x16")), QByteArray(), this);

    createGui();

    connect(
        m_aggregatedAccountAvatarService, &AggregatedAccountAvatarService::finished, this,
        &AccountAvatarWidget::uploadFinished);
    connect(
        m_aggregatedAccountAvatarService, &AggregatedAccountAvatarService::availabilityChanged, this,
        &AccountAvatarWidget::serviceAvailabilityChanged);
    connect(m_avatars, &Avatars::updated, this, &AccountAvatarWidget::avatarUpdated);

    serviceAvailabilityChanged();
}

void AccountAvatarWidget::createGui()
{
    auto layout = make_owned<QVBoxLayout>(this);

    m_avatarLabel = make_owned<QLabel>(this);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setAutoFillBackground(true);
    m_avatarLabel->setBackgroundRole(QPalette::Base);
    m_avatarLabel->setLineWidth(1);
    m_avatarLabel->setFrameShape(static_cast<QFrame::Shape>(QFrame::StyledPanel | QFrame::Sunken));
    m_avatarLabel->setFixedWidth(AVATAR_SIZE + 8);
    m_avatarLabel->setFixedHeight(AVATAR_SIZE + 8);
    m_avatarLabel->setScaledContents(false);

    layout->addWidget(m_avatarLabel, 0, Qt::AlignTop | Qt::AlignRight);

    m_changePhotoButton = make_owned<QPushButton>(this);
    connect(m_changePhotoButton, SIGNAL(clicked(bool)), this, SLOT(changeButtonClicked()));

    layout->addWidget(m_changePhotoButton, 0, Qt::AlignHCenter);

    avatarUpdated(avatarId(m_account.accountContact()));
}

void AccountAvatarWidget::serviceAvailabilityChanged()
{
    m_changePhotoButton->show();

    switch (m_aggregatedAccountAvatarService->availability(m_account))
    {
    case AggregatedAccountAvatarService::Availability::None:
        m_changePhotoButton->hide();
        return;
    case AggregatedAccountAvatarService::Availability::UploadOnly:
        Mode = ModeChange;
        break;
    case AggregatedAccountAvatarService::Availability::Full:
    {
        auto avatar = m_avatars->pixmap(avatarId(m_account.accountContact()));
        Mode = avatar.isNull() ? ModeChange : ModeRemove;
        break;
    }
    }

    if (ModeRemove == Mode)
        m_changePhotoButton->setText(tr("Remove"));
    else
        m_changePhotoButton->setText(tr("Change"));
}

void AccountAvatarWidget::avatarUpdated(const AvatarId &id)
{
    if (id != avatarId(m_account.accountContact()))
        return;

    m_waitMovie->stop();
    m_avatarLabel->setMovie(nullptr);

    auto avatar = m_avatars->pixmap(avatarId(m_account.accountContact()));
    if (!avatar.isNull())
        avatar = avatar.scaled(QSize(AVATAR_SIZE, AVATAR_SIZE), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_avatarLabel->setPixmap(avatar);

    serviceAvailabilityChanged();
}

void AccountAvatarWidget::changeButtonClicked()
{
    if (ModeRemove == Mode)
        removeAvatar();
    else
        changeAvatar();
}

void AccountAvatarWidget::uploadAvatar()
{
    m_avatarLabel->setMovie(m_waitMovie);
    m_waitMovie->start();

    m_aggregatedAccountAvatarService->upload(m_account, m_avatar);

    m_changePhotoButton->setEnabled(false);
}

void AccountAvatarWidget::uploadFinished(const Account &account, bool ok)
{
    if (account != m_account)
        return;

    if (ok)
        m_avatars->update(avatarId(m_account.accountContact()), m_avatar);

    avatarUpdated(avatarId(m_account.accountContact()));
    m_changePhotoButton->setEnabled(true);
}

void AccountAvatarWidget::changeAvatar()
{
    auto fileName = QFileDialog::getOpenFileName(
        this, tr("Select avatar file"), QString(), tr("Images (*.jpeg *.jpg *.png);;All Files (*)"));
    if (fileName.isEmpty())
        return;

    auto avatar = QImage{fileName};
    if (avatar.isNull())
        return;

    m_avatar = QPixmap::fromImage(avatar);
    if (!m_avatar.isNull())
        uploadAvatar();
}

void AccountAvatarWidget::removeAvatar()
{
    m_avatar = {};
    uploadAvatar();
}
