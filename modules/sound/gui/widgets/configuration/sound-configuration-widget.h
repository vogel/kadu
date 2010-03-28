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

#ifndef SOUND_CONFIGURATION_WIDGET_H
#define SOUND_CONFIGURATION_WIDGET_H

#include <QtCore/QMap>

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class QLabel;

class SelectFile;

class SoundConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, QString> SoundFiles;
	QString CurrentNotifyEvent;

	SelectFile *SoundFileSelectFile;
	QLabel *Warning;

private slots:
	void test();

public:
	explicit SoundConfigurationWidget(QWidget *parent = 0);
	virtual ~SoundConfigurationWidget();

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);

public slots:
	void themeChanged(int index);

signals:
	void soundFileEdited();

};

#endif // SOUND_CONFIGURATION_WIDGET_H
