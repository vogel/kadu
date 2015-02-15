/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtXml/QDomDocument>

#include "server/gadu-avatar-downloader.h"
#include "server/gadu-avatar-uploader.h"

#include "gadu-avatar-service.h"

GaduAvatarService::GaduAvatarService(Account account, QObject *parent) :
		AvatarService(account, parent)
{
}

GaduAvatarService::~GaduAvatarService()
{
}

AvatarDownloader * GaduAvatarService::createAvatarDownloader()
{
	return new GaduAvatarDownloader(this);
}

AvatarUploader * GaduAvatarService::createAvatarUploader()
{
	return new GaduAvatarUploader(this);
}

#include "moc_gadu-avatar-service.cpp"
