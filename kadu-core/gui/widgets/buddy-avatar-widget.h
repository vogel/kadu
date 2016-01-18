/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "buddies/buddy.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class AvatarManager;
class BuddyPreferredManager;

class QLabel;
class QPushButton;

class BuddyAvatarWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BuddyAvatarWidget(Buddy buddy, QWidget *parent = 0);

	const QPixmap avatarPixmap();
	bool buddyAvatar() const { return BuddyAvatar; }

private:
	QPointer<AvatarManager> m_avatarManager;
	QPointer<BuddyPreferredManager> m_buddyPreferredManager;

	Buddy MyBuddy;

	bool BuddyAvatar;

	QPixmap BuddyAvatarPixmap;

	QLabel *AvatarLabel;
	QPushButton *ChangePhotoButton;

	void createGui();
	void setupChangeButton();

	void showAvatar();
	void showAvatar(QPixmap pixmap);
	void showBuddyAvatar();
	void showContactAvatar();

	void changeAvatar();
	void removeAvatar();

private slots:
	INJEQT_SET void setAvatarManager(AvatarManager *avatarManager);
	INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
	INJEQT_INIT void init();

	void buttonClicked();

};
