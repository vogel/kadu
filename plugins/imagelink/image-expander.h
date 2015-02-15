/*
 * %kadu copyright begin%
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

#ifndef IMAGE_EXPANDER_H
#define IMAGE_EXPANDER_H

#include "dom/dom-text-regexp-visitor.h"

class ImageExpander : public DomTextRegexpVisitor
{

public:
	ImageExpander();
	virtual ~ImageExpander();

	virtual QList<QDomNode> matchToDomNodes(QDomDocument document, QRegExp regExp);

};

#endif // IMAGE_EXPANDER_H
