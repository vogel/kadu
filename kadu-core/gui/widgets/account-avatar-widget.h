/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef ACCOUNT_AVATAR_WIDGET_H
#define ACCOUNT_AVATAR_WIDGET_H

#include <QtGui/QWidget>

#include "accounts/account.h"
#include "exports.h"

class QLabel;
class QPushButton;

class AvatarService;

class KADUAPI AccountAvatarWidget : public QWidget
{
	Q_OBJECT

	enum
	{
		ModeChange,
		ModeRemove
	} Mode;

	Account MyAccount;
	AvatarService *Service;

	QLabel *AvatarLabel;
	QMovie *WaitMovie;
	QPushButton *ChangePhotoButton;

	void createGui();
	void setupMode();

	void changeAvatar();
	void removeAvatar();

private slots:
	void avatarUpdated();

	void changeButtonClicked();
	void avatarUploaded(bool ok, QImage image);
	void serviceDestroyed();

	void protocolRegistered(ProtocolFactory *protocolFactory);
	void protocolUnregistered(ProtocolFactory *protocolFactory);

public:
	explicit AccountAvatarWidget(Account account, QWidget *parent = 0);
	virtual ~AccountAvatarWidget();

};

#endif // ACCOUNT_AVATAR_WIDGET_H
