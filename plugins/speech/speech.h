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

#include "notification/notifier.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTime>
#include <injeqt/injeqt.h>

class Configuration;
class Notification;
class Parser;

/**
 * @defgroup speech Speech
 * @{
 */

class Speech : public QObject, public Notifier
{
	Q_OBJECT

	QTime lastSpeech;

public:
	Q_INVOKABLE explicit Speech(QObject *parent = nullptr);
	virtual ~Speech();

	virtual void notify(Notification *notification);
	NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent);

	void say(const QString &s,
	const QString &path = QString(),
	bool klatt = false, bool melodie = false,
	const QString &sound_system = QString(), const QString &device = QString(),
	int freq = 0, int tempo = 0, int basefreq = 0);

private:
	QPointer<Configuration> m_configuration;
	QPointer<Parser> m_parser;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_INIT void init();

};

/** @} */
