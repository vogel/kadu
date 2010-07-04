/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class KADUAPI AccountAvatarWidget : public QWidget, ProtocolsAwareObject
{
	Q_OBJECT

	Account MyAccount;
	AvatarService *Service;

	QLabel *AvatarLabel;
	QMovie *WaitMovie;
	QPushButton *ChangeAvatarButton;

	void createGui();

private slots:
	void avatarUpdated();
	void changeAvatar();
	void avatarUploaded(bool ok, QImage image);

protected:
	virtual void protocolRegistered(ProtocolFactory *protocolFactory);
	virtual void protocolUnregistered(ProtocolFactory *protocolFactory);

public:
	explicit AccountAvatarWidget(Account account, QWidget *parent = 0);
	virtual ~AccountAvatarWidget();

};

#endif // ACCOUNT_AVATAR_WIDGET_H
