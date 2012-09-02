/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStack>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtGui/QTextDocument>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "dom/dom-processor.h"
#include "dom/dom-processor-service.h"
#include "dom/ignore-links-dom-visitor.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"
#include "misc/misc.h"
#include "debug.h"

#include "animated-emoticon-path-provider.h"
#include "emoticon.h"
#include "emoticon-expander.h"
#include "emoticon-expander-dom-visitor-provider.h"
#include "emoticon-prefix-tree-builder.h"
#include "emoticon-theme.h"
#include "emoticon-theme-manager.h"
#include "emoticons-configuration-ui-handler.h"
#include "gadu-emoticon-theme-loader.h"
#include "insert-emoticon-action.h"
#include "static-emoticon-path-provider.h"

#include "emoticons-manager.h"

EmoticonsManager::EmoticonsManager(QObject *parent)
{
	Q_UNUSED(parent)

	QStringList iconPaths = config_file.readEntry("Chat", "EmoticonsPaths").split('&', QString::SkipEmptyParts);

	ThemeManager = new EmoticonThemeManager(this);
	ThemeManager->loadThemes(iconPaths);
	ExpanderDomVisitorProvider = new EmoticonExpanderDomVisitorProvider();
	Core::instance()->domProcessorService()->registerVisitorProvider(ExpanderDomVisitorProvider, 2000);
	ConfigurationUiHandler = new EmoticonsConfigurationUiHandler(ThemeManager, this);
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/emoticons.ui"));
	MainConfigurationWindow::registerUiHandler(ConfigurationUiHandler);

	InsertAction = new InsertEmoticonAction(this);
	configurationUpdated();
}

EmoticonsManager::~EmoticonsManager()
{
	MainConfigurationWindow::unregisterUiHandler(ConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/emoticons.ui"));
	Core::instance()->domProcessorService()->unregisterVisitorProvider(ExpanderDomVisitorProvider);
	delete ExpanderDomVisitorProvider;
	ExpanderDomVisitorProvider = 0;
}

EmoticonThemeManager * EmoticonsManager::themeManager() const
{
	return ThemeManager;
}

void EmoticonsManager::configurationUpdated()
{
	bool themeWasChanged = config_file.readEntry("Chat", "EmoticonsTheme") != ThemeManager->currentTheme().name();
	if (themeWasChanged)
	{
		ThemeManager->setCurrentTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
		config_file.writeEntry("Chat", "EmoticonsTheme", ThemeManager->currentTheme().name());

		loadTheme();
	}

	ExpanderDomVisitorProvider->setStyle((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));
}

void EmoticonsManager::loadTheme()
{
	Theme theme = ThemeManager->currentTheme();
	if (theme.isValid())
		loadGGEmoticonTheme(theme.path());
}

void EmoticonsManager::loadGGEmoticonTheme(const QString &themeDirPath)
{
	GaduEmoticonThemeLoader loader;
	Emoticons = loader.loadEmoticonTheme(themeDirPath);

	if (!Emoticons.aliases().isEmpty())
	{
		EmoticonPrefixTreeBuilder builder;
		foreach (const Emoticon &emoticon, Emoticons.aliases())
			builder.addEmoticon(emoticon);

		ExpanderDomVisitorProvider->setEmoticonTree(builder.tree());
	}
	else
		ExpanderDomVisitorProvider->setEmoticonTree(0);

	InsertAction->setEmoticons(Emoticons.emoticons());
}
