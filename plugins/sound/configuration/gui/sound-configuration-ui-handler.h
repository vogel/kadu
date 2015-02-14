/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "gui/windows/main-configuration-window.h"

#include <QtCore/QPointer>

class NotifierConfigurationWidget;
class PathListEdit;
class SoundConfigurationWidget;
class SoundManager;
class SoundThemeManager;

class SoundConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

public:
	explicit SoundConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~SoundConfigurationUiHandler();

	void setManager(SoundManager *manager);
	void setSoundThemeManager(SoundThemeManager *soundThemeManager);

	NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = nullptr);

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;

private:
	QPointer<SoundManager> m_manager;
	QPointer<SoundThemeManager> m_soundThemeManager;

	QPointer<SoundConfigurationWidget> m_configurationWidget;
	QPointer<ConfigComboBox> m_themesComboBox;
	QPointer<PathListEdit> m_themesPaths;

	void connectWidgets();
	void setSoundThemes();

private slots:
	void themeChanged(const QString &theme);
	void soundFileEdited();

	void configurationWindowApplied();

};
