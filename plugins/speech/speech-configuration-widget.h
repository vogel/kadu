/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef SPEECH_CONFIGURATION_WIDGET_H
#define SPEECH_CONFIGURATION_WIDGET_H

#include <QtCore/QMap>

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class QLineEdit;

class SpeechConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, QString> maleFormat;
	QMap<QString, QString> femaleFormat;
	QString currentNotifyEvent;

	QLineEdit *maleLineEdit;
	QLineEdit *femaleLineEdit;

public:
	explicit SpeechConfigurationWidget(QWidget *parent = 0);
	virtual ~SpeechConfigurationWidget();

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

#endif // SPEECH_CONFIGURATION_WIDGET_H
