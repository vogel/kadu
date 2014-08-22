/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef VIDEO_LINK_DOM_VISITOR_PROVIDER_H
#define VIDEO_LINK_DOM_VISITOR_PROVIDER_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include "dom/ignore-links-dom-visitor.h"

#include "configuration/image-link-configuration.h"

#include "dom/dom-visitor-provider.h"

class VideoExpanderDomVisitorProvider : public QObject, public DomVisitorProvider
{
	Q_OBJECT

	ImageLinkConfiguration Configuration;
	QScopedPointer<IgnoreLinksDomVisitor> Visitor;

public:
	VideoExpanderDomVisitorProvider();
	virtual ~VideoExpanderDomVisitorProvider();

	virtual DomVisitor * provide() const;

	void setConfiguration(const ImageLinkConfiguration &configuration);

};

#endif // VIDEO_LINK_DOM_VISITOR_PROVIDER_H
