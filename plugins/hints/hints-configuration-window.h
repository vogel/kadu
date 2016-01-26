/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/windows/configuration-window.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigLabel;
class InjectedFactory;
class NotifierConfigurationDataManager;
class PathsProvider;

class HintsConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

	QPointer<InjectedFactory> m_injectedFactory;	
	QPointer<PathsProvider> m_pathsProvider;

	NotifierConfigurationDataManager *m_dataManager;
	ConfigLabel *preview;

	QString EventName;

	static QMap<QString, HintsConfigurationWindow *> ConfigurationWindows;
	static void windowDestroyed(const QString &eventName);

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

	void fontChanged(QFont font);
	void foregroundColorChanged(const QColor &color);
	void backgroundColorChanged(const QColor &color);

public:
	static HintsConfigurationWindow * configWindowForEvent(InjectedFactory *injectedFactory, const QString &eventName);

	explicit HintsConfigurationWindow(const QString &eventName, NotifierConfigurationDataManager *dataManager);
	~HintsConfigurationWindow();

};
