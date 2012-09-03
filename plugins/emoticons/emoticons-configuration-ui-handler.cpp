/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>

#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/path-list-edit.h"

#include "emoticon-theme-manager.h"
#include "emoticons-manager.h"

#include "emoticons-configuration-ui-handler.h"

EmoticonsConfigurationUiHandler::EmoticonsConfigurationUiHandler(EmoticonThemeManager *themeManager, QObject *parent) :
		ConfigurationUiHandler(parent), ThemeManager(themeManager)
{
}

EmoticonsConfigurationUiHandler::~EmoticonsConfigurationUiHandler()
{
}

void EmoticonsConfigurationUiHandler::updateEmoticonThemes()
{
	if (!Widget || !ThemeManager)
		return;

	ConfigComboBox *emoticonsThemes = static_cast<ConfigComboBox *>(Widget.data()->widgetById("emoticonsTheme"));
	ThemeManager.data()->loadThemes((static_cast<PathListEdit *>(Widget.data()->widgetById("emoticonsPaths")))->pathList());

	(void)QT_TRANSLATE_NOOP("@default", "default");

	QStringList values;
	QStringList captions;
	foreach (const Theme &theme, ThemeManager.data()->themes())
	{
		values.append(theme.name());
		captions.append(qApp->translate("@default", theme.name().toUtf8().constData()));
	}

	emoticonsThemes->setItems(values, captions);
	emoticonsThemes->setCurrentItem(ThemeManager.data()->currentTheme().name());
}

void EmoticonsConfigurationUiHandler::emoticonThemeSelected(int index)
{
	EmoticonsStyleComboBox.data()->setEnabled(index != 0);
	EmoticonsScalingComboBox.data()->setEnabled(index != 0);
}

void EmoticonsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	Widget = mainConfigurationWindow->widget();
	connect(Widget.data()->widgetById("emoticonsPaths"), SIGNAL(changed()), this, SLOT(updateEmoticonThemes()));

	EmoticonsStyleComboBox = static_cast<ConfigComboBox *>(Widget.data()->widgetById("emoticonsStyle"));
	EmoticonsThemeComboBox = static_cast<ConfigComboBox *>(Widget.data()->widgetById("emoticonsTheme"));
	EmoticonsScalingComboBox = static_cast<ConfigComboBox *>(Widget.data()->widgetById("emoticonsScaling"));
	connect(EmoticonsThemeComboBox.data(), SIGNAL(currentIndexChanged(int)), this, SLOT(emoticonThemeSelected(int)));

	updateEmoticonThemes();
}
