/*
 * %kadu copyright begin%
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef HINTS_CONFIGURATION_WINDOW_H
#define HINTS_CONFIGURATION_WINDOW_H

#include "gui/windows/configuration-window.h"

class QLabel;

class NotifierConfigurationDataManager;

class HintsConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

    	QLabel *preview;

	QString EventName;

    	explicit HintsConfigurationWindow(const QString &eventName, NotifierConfigurationDataManager *dataManager);

	static QMap<QString, HintsConfigurationWindow *> ConfigurationWindows;
	static void windowDestroyed(const QString &eventName);

private slots:
	void fontChanged(QFont font);
	void foregroundColorChanged(const QColor &color);
	void backgroundColorChanged(const QColor &color);

public:
	static HintsConfigurationWindow * configWindowForEvent(const QString &eventName);

	~HintsConfigurationWindow();
};

#endif // HINT_SCONFIGURATION_WINDOW_H
