/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "avatars/avatar.h"
#include "buddies/buddy-preferred-manager.h"

#include "buddy-avatar-widget.h"

BuddyAvatarWidget::BuddyAvatarWidget(Buddy buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy), BuddyAvatar(false)
{
	createGui();
}

void BuddyAvatarWidget::showAvatar()
{
	if (MyBuddy.buddyAvatar().pixmap().isNull())
		showContactAvatar();
	else
		showBuddyAvatar();
}

void BuddyAvatarWidget::showAvatar(QPixmap pixmap)
{
	if (!pixmap.isNull())
	{
		if (pixmap.width() > 128 || pixmap.height() > 128)
			pixmap = pixmap.scaled(QSize(128, 128), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	AvatarLabel->setPixmap(pixmap);
}

void BuddyAvatarWidget::showBuddyAvatar()
{
	showAvatar(MyBuddy.buddyAvatar().pixmap());
	BuddyAvatar = true;
}

void BuddyAvatarWidget::showContactAvatar()
{
	Contact preferredContact = BuddyPreferredManager::instance()->preferredContact(MyBuddy);
	showAvatar(preferredContact.contactAvatar().pixmap());
	BuddyAvatar = false;
}

void BuddyAvatarWidget::createGui()
{
	QVBoxLayout *photoLayout = new QVBoxLayout(this);
	photoLayout->setSpacing(2);

	AvatarLabel = new QLabel(this);
	showAvatar();

	photoLayout->addWidget(AvatarLabel, 0, Qt::AlignCenter);

	ChangePhotoButton = new QPushButton(this);
	connect(ChangePhotoButton, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
	photoLayout->addWidget(ChangePhotoButton);

	setupChangeButton();
}

void BuddyAvatarWidget::setupChangeButton()
{
	if (BuddyAvatar)
		ChangePhotoButton->setText(tr("Remove Custom Photo..."));
	else
		ChangePhotoButton->setText(tr("Change Photo..."));
}

void BuddyAvatarWidget::buttonClicked()
{
	if (BuddyAvatar)
		removeAvatar();
	else
		changeAvatar();
}

void BuddyAvatarWidget::changeAvatar()
{
	QString newAvatar = QFileDialog::getOpenFileName(this, tr("Select new photo"), "", "Image Files (*.png *.jpg *.bmp)", 0);
	if (newAvatar.isEmpty())
		return;

	QPixmap pixmap;
	if (pixmap.load(newAvatar))
	{
		showAvatar(newAvatar);
		BuddyAvatarPixmap = pixmap;
		BuddyAvatar = true;
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
	if (BuddyAvatar && !BuddyAvatarPixmap.isNull())
		return BuddyAvatarPixmap;

	return QPixmap();
}
