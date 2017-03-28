/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "configuration/configuration-holder.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class ImageExpanderDomVisitorProvider;
class VideoExpanderDomVisitorProvider;

class ImageLinkConfigurator : public ConfigurationHolder
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ImageLinkConfigurator(QObject *parent = nullptr);
    virtual ~ImageLinkConfigurator();

    void configure();

protected:
    virtual void configurationUpdated();

private:
    QPointer<Configuration> m_configuration;
    QPointer<ImageExpanderDomVisitorProvider> m_imageExpander;
    QPointer<VideoExpanderDomVisitorProvider> m_videoExpander;

    void createDefaultConfiguration();

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setImageExpanderProvider(ImageExpanderDomVisitorProvider *imageExpander);
    INJEQT_SET void setVideoExpanderProvider(VideoExpanderDomVisitorProvider *videoExpander);
    INJEQT_INIT void init();
};
