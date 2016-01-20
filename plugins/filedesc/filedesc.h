/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
class FileDescStatusChanger;
class PathsProvider;

class QTimer;

class FileDescription : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit FileDescription(QObject *parent = nullptr);
	virtual ~FileDescription();

	bool allowOther() const { return m_allowOther; }
	bool forceDesc() const { return m_forceDesc; }

public slots:
	void checkTitle();

protected:
	virtual void configurationUpdated();

private:
	QPointer<Configuration> m_configuration;
	QPointer<FileDescStatusChanger> m_fileDescStatusChanger;
	QPointer<PathsProvider> m_pathsProvider;

	QTimer *m_timer;

	QString m_file;
	bool m_allowOther;
	bool m_forceDesc;

	void createDefaultConfiguration();

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setFileDescStatusChanger(FileDescStatusChanger *fileDescStatusChanger);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

};
