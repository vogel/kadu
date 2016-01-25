/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <memory>

class AccountStorage;
class AvatarManager;
class BuddyStorage;
class Buddy;
class IconsManager;
class InjectedFactory;
class StatusTypeManager;
class StoragePoint;

class KADUAPI BuddyDummyFactory : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit BuddyDummyFactory(QObject *parent = nullptr);
	virtual ~BuddyDummyFactory();

	Buddy dummy();

private:
	QPointer<AccountStorage> m_accountStorage;
	QPointer<AvatarManager> m_avatarManager;
	QPointer<BuddyStorage> m_buddyStorage;
	QPointer<IconsManager> m_iconsManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<StatusTypeManager> m_statusTypeManager;

private slots:
	INJEQT_SET void setAccountStorage(AccountStorage *accountStorage);
	INJEQT_SET void setAvatarManager(AvatarManager *avatarManager);
	INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);

};
