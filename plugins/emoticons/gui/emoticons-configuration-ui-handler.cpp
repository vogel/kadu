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

#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/path-list-edit.h"

#include "emoticons-configuration-ui-handler.h"

EmoticonsConfigurationUiHandler::EmoticonsConfigurationUiHandler(QObject *parent) :
		ConfigurationUiHandler(parent), ThemeManager(new EmoticonThemeManager())
{
}

EmoticonsConfigurationUiHandler::~EmoticonsConfigurationUiHandler()
{
}

void EmoticonsConfigurationUiHandler::updateEmoticonThemes()
{
	if (!EmoticonsThemeComboBox || !EmoticonsThemesPathListEdit)
		return;

	ThemeManager.data()->loadThemes(EmoticonsThemesPathListEdit.data()->pathList());

	(void)QT_TRANSLATE_NOOP("@default", "default");

	QStringList values;
	QStringList captions;
	foreach (const Theme &theme, ThemeManager.data()->themes())
	{
		values.append(theme.name());
		captions.append(qApp->translate("@default", theme.name().toUtf8().constData()));
	}

	EmoticonsThemeComboBox.data()->setItems(values, captions);
	EmoticonsThemeComboBox.data()->setCurrentItem(ThemeManager.data()->currentTheme().name());
}

void EmoticonsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	Widget = mainConfigurationWindow->widget();

	EmoticonsThemeComboBox = static_cast<ConfigComboBox *>(Widget.data()->widgetById("emoticonsTheme"));
	EmoticonsThemesPathListEdit = static_cast<ConfigPathListEdit *>(Widget.data()->widgetById("emoticonsPaths"));

	QWidget *enableWidget = Widget.data()->widgetById("enableEmoticons");
	connect(enableWidget, SIGNAL(toggled(bool)), Widget.data()->widgetById("emoticonsStyle"), SLOT(setEnabled(bool)));
	connect(enableWidget, SIGNAL(toggled(bool)), EmoticonsThemeComboBox.data(), SLOT(setEnabled(bool)));
	connect(enableWidget, SIGNAL(toggled(bool)), EmoticonsThemesPathListEdit.data(), SLOT(setEnabled(bool)));

	connect(EmoticonsThemesPathListEdit.data(), SIGNAL(changed()), this, SLOT(updateEmoticonThemes()));

	updateEmoticonThemes();
}
