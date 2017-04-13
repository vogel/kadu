/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddy-avatar-widget.h"
#include "buddy-avatar-widget.moc"

#include "avatars/avatar-id.h"
#include "avatars/avatars.h"
#include "buddies/buddy-preferred-manager.h"

#include <QtGui/QImageReader>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

BuddyAvatarWidget::BuddyAvatarWidget(Buddy buddy, QWidget *parent) : QWidget{parent}, m_buddy{buddy}, m_avatarSet(false)
{
}

void BuddyAvatarWidget::setAvatars(Avatars *avatars)
{
    m_avatars = avatars;
}

void BuddyAvatarWidget::setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager)
{
    m_buddyPreferredManager = buddyPreferredManager;
}

void BuddyAvatarWidget::init()
{
    createGui();
}

void BuddyAvatarWidget::showAvatar()
{
    if (m_avatars->pixmap(avatarId(m_buddy)).isNull())
        showContactAvatar();
    else
        showBuddyAvatar();
}

void BuddyAvatarWidget::showAvatar(QPixmap pixmap)
{
    pixmap = pixmap.scaled(QSize{AVATAR_SIZE, AVATAR_SIZE}, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_avatarLabel->setPixmap(pixmap);
}

void BuddyAvatarWidget::showBuddyAvatar()
{
    showAvatar(m_avatars->pixmap(avatarId(m_buddy)));
    m_avatarSet = true;
}

void BuddyAvatarWidget::showContactAvatar()
{
    auto preferredContact = m_buddyPreferredManager->preferredContact(m_buddy);
    showAvatar(m_avatars->pixmap(avatarId(preferredContact)));
    m_avatarSet = false;
}

void BuddyAvatarWidget::createGui()
{
    auto photoLayout = make_owned<QVBoxLayout>(this);
    photoLayout->setSpacing(2);

    m_avatarLabel = make_owned<QLabel>(this);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setAutoFillBackground(true);
    m_avatarLabel->setBackgroundRole(QPalette::Base);
    m_avatarLabel->setLineWidth(1);
    m_avatarLabel->setFrameShape(static_cast<QFrame::Shape>(QFrame::StyledPanel | QFrame::Sunken));
    m_avatarLabel->setFixedWidth(AVATAR_SIZE + 8);
    m_avatarLabel->setFixedHeight(AVATAR_SIZE + 8);
    m_avatarLabel->setScaledContents(false);

    showAvatar();

    photoLayout->addWidget(m_avatarLabel, 0, Qt::AlignTop | Qt::AlignRight);

    m_changeAvatarButton = make_owned<QPushButton>(this);
    connect(m_changeAvatarButton, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
    photoLayout->addWidget(m_changeAvatarButton);

    setupChangeButton();
}

void BuddyAvatarWidget::setupChangeButton()
{
    if (m_avatarSet)
        m_changeAvatarButton->setText(tr("Remove"));
    else
        m_changeAvatarButton->setText(tr("Change"));
}

void BuddyAvatarWidget::buttonClicked()
{
    if (m_avatarSet)
        removeAvatar();
    else
        changeAvatar();
}

void BuddyAvatarWidget::changeAvatar()
{
    auto newAvatar = QFileDialog::getOpenFileName(
        this, tr("Select new photo"), QString(), tr("Images (*.png *.jpg *.bmp);;All Files (*)"), 0);
    if (newAvatar.isEmpty())
        return;

    QImageReader imageReader(newAvatar);
    QPixmap pixmap = QPixmap::fromImageReader(&imageReader);

    if (!pixmap.isNull())
    {
        showAvatar(newAvatar);
        m_avatar = pixmap;
        m_avatarSet = true;
    }

    setupChangeButton();
}

void BuddyAvatarWidget::removeAvatar()
{
    showContactAvatar();
    setupChangeButton();
}

const QPixmap BuddyAvatarWidget::avatarPixmap()
{
    if (m_avatarSet && !m_avatar.isNull())
        return m_avatar;

    return QPixmap{};
}
