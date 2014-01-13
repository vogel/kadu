/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICONS_PLUGIN_H
#define EMOTICONS_PLUGIN_H

#include <QtCore/QObject>

#include "plugin/plugin-root-component.h"

class EmoticonsConfigurationUiHandler;
class EmoticonClipboardHtmlTransformer;
class EmoticonConfigurator;
class EmoticonExpanderDomVisitorProvider;
class InsertEmoticonAction;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonsPlugin
 * @short Emoticons plugin initialization and finalization class.
 * @author Rafał 'Vogel' Malinowski'
 */
class EmoticonsPlugin : public QObject, public PluginRootComponent
{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

	QScopedPointer<EmoticonsConfigurationUiHandler> ConfigurationUiHandler;
	QScopedPointer<EmoticonExpanderDomVisitorProvider> ExpanderDomVisitorProvider;
	QScopedPointer<EmoticonClipboardHtmlTransformer> ClipboardTransformer;
	QScopedPointer<InsertEmoticonAction> InsertAction;
	QScopedPointer<EmoticonConfigurator> Configurator;

	void registerConfigurationUi();
	void unregisterConfigurationUi();

	void registerEmoticonExpander();
	void unregisterEmoticonExpander();

	void registerEmoticonClipboardHtmlTransformer();
	void unregisterEmoticonClipboardHtmlTransformer();

	void registerActions();
	void unregisterActions();

	void startConfigurator();
	void stopConfigurator();

public:
	explicit EmoticonsPlugin(QObject *parent = 0);
	virtual ~EmoticonsPlugin();

	virtual bool init(bool firstLoad);
	virtual void done();

};

/**
 * @}
 */

#endif // EMOTICONS_PLUGIN_H
