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

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "dom/dom-processor-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"

#include "emoticon-configurator.h"
#include "emoticon-expander-dom-visitor-provider.h"
#include "emoticon-prefix-tree-builder.h"
#include "emoticon-theme.h"
#include "emoticon-theme-manager.h"
#include "emoticons-configuration-ui-handler.h"
#include "gadu-emoticon-theme-loader.h"
#include "insert-emoticon-action.h"

#include "emoticons-manager.h"

EmoticonsManager::EmoticonsManager(QObject *parent) :
		QObject(parent)
{
	QStringList iconPaths = config_file.readEntry("Chat", "EmoticonsPaths").split('&', QString::SkipEmptyParts);

	ThemeManager = new EmoticonThemeManager(this);
	ThemeManager->loadThemes(iconPaths);
	ExpanderDomVisitorProvider = new EmoticonExpanderDomVisitorProvider();
	Core::instance()->domProcessorService()->registerVisitorProvider(ExpanderDomVisitorProvider, 2000);
	ConfigurationUiHandler = new EmoticonsConfigurationUiHandler(ThemeManager, this);
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/emoticons.ui"));
	MainConfigurationWindow::registerUiHandler(ConfigurationUiHandler);

	InsertAction = new InsertEmoticonAction(this);
	Configurator.reset(new EmoticonConfigurator(ThemeManager));
	Configurator.data()->setEmoticonsManager(this);
}

EmoticonsManager::~EmoticonsManager()
{
	MainConfigurationWindow::unregisterUiHandler(ConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/emoticons.ui"));
	Core::instance()->domProcessorService()->unregisterVisitorProvider(ExpanderDomVisitorProvider);
	delete ExpanderDomVisitorProvider;
	ExpanderDomVisitorProvider = 0;
}

void EmoticonsManager::setConfiguration(const EmoticonConfiguration &configuration)
{
	bool themeChanged = Configuration.emoticonTheme() != configuration.emoticonTheme();
	Configuration = configuration;

	if (themeChanged)
		loadGGEmoticonTheme(Configuration.emoticonTheme().path());
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
