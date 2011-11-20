/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/path-list-edit.h"
#include "misc/path-conversion.h"
#include "debug.h"
#include "themes.h"

#include "gui/widgets/configuration/sound-configuration-widget.h"
#include "sound-manager.h"
#include "sound-theme-manager.h"

#include "sound-configuration-ui-handler.h"

SoundConfigurationUiHandler *SoundConfigurationUiHandler::Instance = 0;

void SoundConfigurationUiHandler::registerConfigurationUi()
{
	if (Instance)
		return;

	Instance = new SoundConfigurationUiHandler();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/sound.ui"));
	MainConfigurationWindow::registerUiHandler(Instance);
}

void SoundConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
		MainConfigurationWindow::unregisterUiHandler(Instance);

	delete Instance;
	Instance = 0;

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/sound.ui"));
}

SoundConfigurationUiHandler * SoundConfigurationUiHandler::instance()
{
	return Instance;
}

SoundConfigurationUiHandler::SoundConfigurationUiHandler(QObject *parent) :
		ConfigurationUiHandler(), ConfigurationWidget(0), ThemesComboBox(0)
{
	Q_UNUSED(parent)
}

SoundConfigurationUiHandler::~SoundConfigurationUiHandler()
{
}

void SoundConfigurationUiHandler::setSoundThemes()
{
	SoundThemeManager::instance()->themes()->setPaths(ThemesPaths->pathList());

	QStringList soundThemeNames = SoundThemeManager::instance()->themes()->themes();
	soundThemeNames.sort();

	QStringList soundThemeValues = soundThemeNames;

	soundThemeNames.prepend(tr("Custom"));
	soundThemeValues.prepend("Custom");

	ThemesComboBox->setItems(soundThemeValues, soundThemeNames);
	ThemesComboBox->setCurrentIndex(ThemesComboBox->findText(SoundThemeManager::instance()->themes()->theme()));
}

void SoundConfigurationUiHandler::connectWidgets()
{
	if (ThemesComboBox && ConfigurationWidget)
	{
		connect(ThemesComboBox, SIGNAL(activated(int)), ConfigurationWidget, SLOT(themeChanged(int)));
		connect(ThemesComboBox, SIGNAL(activated(const QString &)), this, SLOT(themeChanged(const QString &)));
		ConfigurationWidget->themeChanged(ThemesComboBox->currentIndex());
	}
}

void SoundConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(destroyed()), this, SLOT(configurationWindowDestroyed()));

	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	connect(mainConfigurationWindow->widget()->widgetById("sound/use"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("sound/theme"), SLOT(setEnabled(bool)));
	//connect(mainConfigurationWindow->widget()->widgetById("sound/use"), SIGNAL(toggled(bool)),
	//	mainConfigurationWindow->widget()->widgetById("sound/samples"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("sound/testPlay"), SIGNAL(clicked()), SoundManager::instance(), SLOT(testSoundPlaying()));

	ThemesComboBox = static_cast<ConfigComboBox *>(mainConfigurationWindow->widget()->widgetById("sound/themes"));
	ThemesPaths = static_cast<PathListEdit *>(mainConfigurationWindow->widget()->widgetById("soundPaths"));
	//connect(ThemesPaths, SIGNAL(changed()), SoundManager::instance(), SLOT(setSoundThemes()));

	setSoundThemes();

	connectWidgets();
}

NotifierConfigurationWidget * SoundConfigurationUiHandler::createConfigurationWidget(QWidget *parent)
{
	ConfigurationWidget = new SoundConfigurationWidget(parent);
	connect(ConfigurationWidget, SIGNAL(soundFileEdited()), this, SLOT(soundFileEdited()));

	connectWidgets();

	return ConfigurationWidget;
}

void SoundConfigurationUiHandler::themeChanged(const QString &theme)
{
	SoundThemeManager::instance()->applyTheme(theme);
}

void SoundConfigurationUiHandler::soundFileEdited()
{
	if (ThemesComboBox->currentIndex() != 0)
		ThemesComboBox->setCurrentIndex(0);
}

void SoundConfigurationUiHandler::configurationWindowApplied()
{
	kdebugf();

	if (ThemesComboBox->currentIndex() != 0)
		SoundThemeManager::instance()->applyTheme(ThemesComboBox->currentText());

	ConfigurationWidget->themeChanged(ThemesComboBox->currentIndex());
}

void SoundConfigurationUiHandler::configurationWindowDestroyed()
{
	ThemesComboBox = 0;
	ConfigurationWidget = 0;
}
