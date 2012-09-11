/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONVERT_TO_CLIPBOARD_DOM_VISITOR_H
#define CONVERT_TO_CLIPBOARD_DOM_VISITOR_H

#include "dom/dom-visitor.h"

class ConvertToClipboardDomVisitor : public DomVisitor
{
	virtual QDomNode visitImg(QDomElement imgElement);
	virtual QDomNode visitA(QDomElement aElement);

public:
	ConvertToClipboardDomVisitor();
	virtual ~ConvertToClipboardDomVisitor();

	virtual QDomNode visit(QDomText textNode);
	virtual QDomNode beginVisit(QDomElement elementNode);
	virtual QDomNode endVisit(QDomElement elementNode);

};


#endif // CONVERT_TO_CLIPBOARD_DOM_VISITOR_H
