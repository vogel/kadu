/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/application.h"
#include "core/core.h"
#include "dom/dom-processor-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

#include "image-expander-dom-visitor-provider.h"
#include "video-expander-dom-visitor-provider.h"

#include "image-link-plugin.h"

ImageLinkPlugin::ImageLinkPlugin(QObject *parent) :
		QObject(parent)
{
}

ImageLinkPlugin::~ImageLinkPlugin()
{
}

void ImageLinkPlugin::registerConfigurationUi()
{
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/image-link.ui"));
}

void ImageLinkPlugin::unregisterConfigurationUi()
{
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/image-link.ui"));
}

void ImageLinkPlugin::registerImageExpander()
{
	// do it before all url providers
	ImageExpanderProvider.reset(new ImageExpanderDomVisitorProvider());
	Core::instance()->domProcessorService()->registerVisitorProvider(ImageExpanderProvider.data(), -100);
}

void ImageLinkPlugin::unregisterImageExpander()
{
	Core::instance()->domProcessorService()->unregisterVisitorProvider(ImageExpanderProvider.data());
	ImageExpanderProvider.reset();
}

void ImageLinkPlugin::registerVideoExpander()
{
	// do it before all url providers
	VideoExpanderProvider.reset(new VideoExpanderDomVisitorProvider());
	Core::instance()->domProcessorService()->registerVisitorProvider(VideoExpanderProvider.data(), -50);
}

void ImageLinkPlugin::unregisterVideoExpander()
{
	Core::instance()->domProcessorService()->unregisterVisitorProvider(VideoExpanderProvider.data());
	VideoExpanderProvider.reset();
}

void ImageLinkPlugin::startConfigurator()
{
	Configurator.reset(new ImageLinkConfigurator(this));
	Configurator->setImageExpanderProvider(ImageExpanderProvider.data());
	Configurator->setVideoExpanderProvider(VideoExpanderProvider.data());
	Configurator->configure();
}

void ImageLinkPlugin::stopConfigurator()
{
	Configurator.reset();
}

bool ImageLinkPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	registerConfigurationUi();
	registerImageExpander();
	registerVideoExpander();
	startConfigurator();

	return true;
}

void ImageLinkPlugin::done()
{
	stopConfigurator();
	unregisterConfigurationUi();
	unregisterVideoExpander();
	unregisterImageExpander();
}

Q_EXPORT_PLUGIN2(imagelink, ImageLinkPlugin)

#include "moc_image-link-plugin.cpp"
