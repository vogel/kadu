/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef IMAGE_LINK_PLUGIN_H
#define IMAGE_LINK_PLUGIN_H

#include <QtCore/QScopedPointer>

#include "configuration/image-link-configurator.h"
#include "image-expander-dom-visitor-provider.h"
#include "video-expander-dom-visitor-provider.h"

#include "plugin/plugin-root-component.h"

class ImageLinkPlugin : public QObject, public PluginRootComponent
{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

	QScopedPointer<ImageExpanderDomVisitorProvider> ImageExpanderProvider;
	QScopedPointer<VideoExpanderDomVisitorProvider> VideoExpanderProvider;
	QScopedPointer<ImageLinkConfigurator> Configurator;

	void registerConfigurationUi();
	void unregisterConfigurationUi();

	void registerImageExpander();
	void unregisterImageExpander();

	void registerVideoExpander();
	void unregisterVideoExpander();

	void startConfigurator();
	void stopConfigurator();

public:
	explicit ImageLinkPlugin(QObject *parent = 0);
	virtual ~ImageLinkPlugin();

	virtual bool init(bool firstLoad);
	virtual void done();

};

#endif // IMAGE_LINK_PLUGIN_H
