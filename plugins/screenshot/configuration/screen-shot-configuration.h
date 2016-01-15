/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-aware-object.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class PathsProvider;

class ScreenShotConfiguration : public QObject, private ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ScreenShotConfiguration(QObject *parent = nullptr);
	virtual ~ScreenShotConfiguration();

	const QString & fileFormat() const { return FileFormat; }
	bool useShortJpgExtension() { return UseShortJpgExtension; }
	int quality() { return Quality; }
	const QString & imagePath() const { return ImagePath; }
	const QString & fileNamePrefix() const { return FileNamePrefix; }
	bool pasteImageClauseIntoChatWidget() { return PasteImageClauseIntoChatWidget; }
	bool warnAboutDirectorySize() { return WarnAboutDirectorySize; }
	long directorySizeLimit() { return DirectorySizeLimit; }

	QString screenshotFileNameExtension();

protected:
	virtual void configurationUpdated();

private:
	QPointer<Configuration> m_configuration;
	QPointer<PathsProvider> m_pathsProvider;

	QString FileFormat;
	bool UseShortJpgExtension;
	int Quality;
	QString ImagePath;
	QString FileNamePrefix;
	bool PasteImageClauseIntoChatWidget;
	bool WarnAboutDirectorySize;
	long DirectorySizeLimit;

	void createDefaultConfiguration();

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

};
