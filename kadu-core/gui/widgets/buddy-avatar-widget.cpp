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

#include "buddies/avatar.h"

#include "buddy-avatar-widget.h"

BuddyAvatarWidget::BuddyAvatarWidget(Buddy buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy), BuddyAvatarIsShown(false)
{
	createGui();
}

void BuddyAvatarWidget::createGui()
{
	QVBoxLayout *photoLayout = new QVBoxLayout(this);
	photoLayout->setSpacing(2);

	AvatarLabel = new QLabel(this);

	QPixmap pixmap;
	if (!MyBuddy.buddyAvatar().pixmap().isNull())
	{
		pixmap = MyBuddy.buddyAvatar().pixmap();
		BuddyAvatarIsShown = true;
	}
	else if (MyBuddy.preferredContact() && !MyBuddy.preferredContact().contactAvatar().pixmap().isNull())
		pixmap = MyBuddy.preferredContact().contactAvatar().pixmap();

	if (!pixmap.isNull())
	{
		if (pixmap.width() > 128 || pixmap.height() > 128)
			pixmap = pixmap.scaled(QSize(128, 128), Qt::KeepAspectRatio, Qt::SmoothTransformation);

		AvatarLabel->setPixmap(pixmap);
	}

//	AvatarLabel->setFixedSize(QSize(128, 128));
	photoLayout->addWidget(AvatarLabel, 0, Qt::AlignCenter);

	QPushButton *changePhotoButton = new QPushButton(tr("Change Photo..."));
	connect(changePhotoButton, SIGNAL(clicked(bool)), this, SLOT(changeAvatar()));
	photoLayout->addWidget(changePhotoButton);

}

void BuddyAvatarWidget::changeAvatar()
{
	QString newAvatar = QFileDialog::getOpenFileName(this, tr("Select new photo"), "", "Image Files (*.png *.jpg *.bmp)", 0);
	if (newAvatar.isEmpty())
		return;

	QPixmap pixmap;
	if (pixmap.load(newAvatar))
	{
		if (pixmap.width() > 128 || pixmap.height() > 128)
			pixmap = pixmap.scaled(QSize(128, 128), Qt::KeepAspectRatio, Qt::SmoothTransformation);

		AvatarLabel->setPixmap(pixmap);

		BuddyAvatarIsShown = true;
	}
}

const QPixmap BuddyAvatarWidget::avatarPixmap()
{
	if (!BuddyAvatarIsShown)
		return QPixmap();

	const QPixmap *avatar = AvatarLabel->pixmap();

	if (avatar && !avatar->isNull())
		return *avatar;

	return QPixmap();
}
