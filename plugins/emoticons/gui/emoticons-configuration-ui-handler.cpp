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
#include <QtGui/QPainter>

#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/config-list-widget.h"
#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/path-list-edit.h"

#include "theme/emoticon-theme.h"
#include "theme/gadu-emoticon-theme-loader.h"

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
	if (!ThemesList || !EmoticonsThemesPathListEdit)
		return;

	ThemeManager.data()->loadThemes(EmoticonsThemesPathListEdit.data()->pathList());

	(void)QT_TRANSLATE_NOOP("@default", "default");

	QStringList values;
	QStringList captions;
	int iconsNumber = 4;
	GaduEmoticonThemeLoader loader;

	QList<QIcon> icons;
	foreach (const Theme &theme, ThemeManager.data()->themes())
	{
		values.append(theme.name());
		captions.append(qApp->translate("@default", theme.name().toUtf8().constData()));

		QPixmap combinedIcon(iconsNumber * 36, 36);
		combinedIcon.fill(Qt::transparent);

		QPainter iconPainter(&combinedIcon);

		for (int i = 0; i < iconsNumber; i++)
		{
			EmoticonTheme emoticonsTheme = loader.loadEmoticonTheme(theme.path());
			Emoticon result = emoticonsTheme.emoticons().at(i);
			QIcon icon(QPixmap(result.staticFilePath()));
			icon.paint(&iconPainter, 2 + 36 * i, 2, 32, 32);
		}

		icons.append(QIcon(combinedIcon));
	}

	ThemesList.data()->setItems(values, captions);
	ThemesList.data()->setCurrentItem(ThemeManager.data()->currentTheme().name());
	ThemesList.data()->setIconSize(QSize(iconsNumber * 36, 36));
	ThemesList.data()->setIcons(icons);
}

void EmoticonsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	Widget = mainConfigurationWindow->widget();

	ThemesList = static_cast<ConfigListWidget *>(Widget.data()->widgetById("emoticonsTheme"));
	EmoticonsThemesPathListEdit = static_cast<ConfigPathListEdit *>(Widget.data()->widgetById("emoticonsPaths"));

	connect(EmoticonsThemesPathListEdit.data(), SIGNAL(changed()), this, SLOT(updateEmoticonThemes()));

	updateEmoticonThemes();
}
