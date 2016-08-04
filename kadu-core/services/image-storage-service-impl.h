/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "services/image-storage-service.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class IconsManager;
class PathsProvider;

class QUrl;

class KADUAPI ImageStorageServiceImpl : public ImageStorageService
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ImageStorageServiceImpl(QObject *parent = nullptr);
	virtual ~ImageStorageServiceImpl();

	virtual QString storagePath() const override;
	virtual QString fullPath(const QString &imageFilePath) override;
	virtual QString storeImage(const QString &imageFilePath) override;
	virtual QString storeImage(const QString &imageFileName, const QByteArray &content) override;
	virtual QUrl toFileUrl(const QUrl &url) override;

private:
	QPointer<IconsManager> m_iconsManager;
	QPointer<PathsProvider> m_pathsProvider;
	QString m_storagePath;

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

};
