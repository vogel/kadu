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

#ifndef HTML_FIXUP_DOM_VISITOR_H
#define HTML_FIXUP_DOM_VISITOR_H

#include "dom-visitor.h"

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class HtmlFixupDomVisitor
 * @short Visitor that allows treating HTML documents as XML.
 * @author Rafał 'Vogel' Malinowski
 *
 * This visitor fixes some bugs that are result of differences between HTML and XML code.
 * Following fixes are performed:
 * <ul>
 *   <li>&lt;img src=....&gt; tags are converted to &lt;img src=..../&gt;
 *   <li>&lt;span class="Apple-converted-space"/&gt; are converted to &amp;nbsp;
 * </ul>
 */
class HtmlFixupDomVisitor : public DomVisitor
{
	QDomNode fixupImg(QDomElement imgNode);
	QDomNode fixupSpan(QDomElement spanNode);

public:
	HtmlFixupDomVisitor();
	virtual ~HtmlFixupDomVisitor();

	virtual QDomNode visit(QDomText textNode);
	virtual QDomNode beginVisit(QDomElement elementNode);
	virtual QDomNode endVisit(QDomElement elementNode);

};

/**
 * @}
 */

#endif // HTML_FIXUP_DOM_VISITOR_H
