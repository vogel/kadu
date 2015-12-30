/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "image-link-plugin-object.h"

#include "configuration/image-link-configurator.h"
#include "image-expander-dom-visitor-provider.h"
#include "video-expander-dom-visitor-provider.h"

#include "dom/dom-processor-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

ImageLinkPluginObject::ImageLinkPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

ImageLinkPluginObject::~ImageLinkPluginObject()
{
}

void ImageLinkPluginObject::setDomProcessorService(DomProcessorService *domProcessorService)
{
	m_domProcessorService = domProcessorService;
}

void ImageLinkPluginObject::setImageExpanderDomVisitorProvider(ImageExpanderDomVisitorProvider *imageExpanderDomVisitorProvider)
{
	m_imageExpanderDomVisitorProvider = imageExpanderDomVisitorProvider;
}

void ImageLinkPluginObject::setImageLinkConfigurator(ImageLinkConfigurator *imageLinkConfigurator)
{
	m_imageLinkConfigurator = imageLinkConfigurator;
}

void ImageLinkPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void ImageLinkPluginObject::setVideoExpanderDomVisitorProvider(VideoExpanderDomVisitorProvider *videoExpanderDomVisitorProvider)
{
	m_videoExpanderDomVisitorProvider = videoExpanderDomVisitorProvider;
}

void ImageLinkPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/image-link.ui"));
	m_domProcessorService->registerVisitorProvider(m_imageExpanderDomVisitorProvider, -100);
	m_domProcessorService->registerVisitorProvider(m_videoExpanderDomVisitorProvider, -50);
}

void ImageLinkPluginObject::done()
{
	m_domProcessorService->unregisterVisitorProvider(m_videoExpanderDomVisitorProvider);
	m_domProcessorService->unregisterVisitorProvider(m_imageExpanderDomVisitorProvider);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/image-link.ui"));
}

#include "moc_image-link-plugin-object.cpp"
