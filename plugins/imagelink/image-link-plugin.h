/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#include "plugins/generic-plugin.h"

class ImageLinkPlugin : public QObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

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

	virtual int init(bool firstLoad);
	virtual void done();

};

#endif // IMAGE_LINK_PLUGIN_H
