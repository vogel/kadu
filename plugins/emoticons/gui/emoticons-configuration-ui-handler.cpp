/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>

#include "compression/archive-extractor.h"
#include "misc/paths-provider.h"
#include "plugin/plugin-injected-factory.h"
#include "widgets/configuration/config-check-box.h"
#include "widgets/configuration/config-list-widget.h"
#include "widgets/configuration/config-path-list-edit.h"
#include "widgets/configuration/configuration-widget.h"
#include "widgets/path-list-edit.h"
#include "windows/main-configuration-window.h"
#include "windows/message-dialog.h"

#include "theme/emoticon-theme.h"
#include "theme/gadu-emoticon-theme-loader.h"

#include "emoticons-configuration-ui-handler.h"
#include "emoticons-configuration-ui-handler.moc"

EmoticonsConfigurationUiHandler::EmoticonsConfigurationUiHandler(QObject *parent) : QObject(parent)
{
}

EmoticonsConfigurationUiHandler::~EmoticonsConfigurationUiHandler()
{
}

void EmoticonsConfigurationUiHandler::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void EmoticonsConfigurationUiHandler::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void EmoticonsConfigurationUiHandler::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void EmoticonsConfigurationUiHandler::init()
{
    ThemeManager.reset(m_pluginInjectedFactory->makeInjected<EmoticonThemeManager>());
}

void EmoticonsConfigurationUiHandler::updateEmoticonThemes()
{
    if (!ThemesList)
        return;

    ThemeManager->loadThemes();

    (void)QT_TRANSLATE_NOOP("@default", "default");

    QStringList values;
    QStringList captions;
    int iconsNumber = 4;

    QList<QIcon> icons;
    for (auto const &theme : ThemeManager->themes())
    {
        values.append(theme.name());
        captions.append(QCoreApplication::translate("@default", theme.name().toUtf8().constData()));

        QPixmap combinedIcon(iconsNumber * 36, 36);
        combinedIcon.fill(Qt::transparent);

        QPainter iconPainter(&combinedIcon);

        for (int i = 0; i < iconsNumber; i++)
        {
            GaduEmoticonThemeLoader loader;
            EmoticonTheme emoticonsTheme = loader.loadEmoticonTheme(theme.path());
            Emoticon result = emoticonsTheme.emoticons().at(i);
            QIcon icon(QPixmap(result.staticFilePath()));
            icon.paint(&iconPainter, 2 + 36 * i, 2, 32, 32);
        }

        icons.append(QIcon(combinedIcon));
    }

    ThemesList->setItems(values, captions);
    ThemesList->setCurrentItem(ThemeManager->currentTheme().name());
    ThemesList->setIconSize(QSize(iconsNumber * 36, 36));
    ThemesList->setIcons(icons);
}

void EmoticonsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
    Widget = mainConfigurationWindow->widget();

    ThemesList = static_cast<ConfigListWidget *>(Widget->widgetById("emoticonsTheme"));
    connect(Widget->widgetById("installEmoticonTheme"), SIGNAL(clicked()), this, SLOT(installEmoticonTheme()));

    updateEmoticonThemes();
}

void EmoticonsConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void EmoticonsConfigurationUiHandler::mainConfigurationWindowApplied()
{
}

void EmoticonsConfigurationUiHandler::installEmoticonTheme()
{
    QString fileName = QFileDialog::getOpenFileName(
        Widget.data(), tr("Open icon theme archive"), QDir::home().path(),
        tr("Archive (*.tar.xz *.tar.gz *.tar.bz2 *.tar *.zip)"));

    if (fileName.isEmpty())
        return;

    const QString &profilePath = m_pathsProvider->profilePath();
    ArchiveExtractor extractor;
    bool success = extractor.extract(fileName, profilePath + "emoticons");

    if (success)
    {
        updateEmoticonThemes();
    }
    else
    {
        MessageDialog::show(
            m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Installation failed"),
            tr(extractor.message().toLocal8Bit().data()), QMessageBox::Ok, Widget.data());
    }
}
