/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef DOM_TEXT_REGEXP_VISITOR_H
#define DOM_TEXT_REGEXP_VISITOR_H

#include <QtCore/QRegExp>

#include "dom/dom-visitor.h"
#include "exports.h"

class QDomDocument;
class QDomNode;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomTextRegexpVisitor
 * @short Visitor that splits text nodes by given regexp and inserts new node in place of matched strings.
 * @author Rafał 'Vogel' Malinowski
 *
 * This visitor is used to find regexp matches in text nodes and insert new nodes in place of them.
 * Override matchToDomNodes() to get each regexp match and create DOM node for it.
 *
 * This visitor is subclassed by emoticon and link expanders.
 */
class KADUAPI DomTextRegexpVisitor : public DomVisitor
{
	QRegExp RegExp;

	QDomText expandFirstMatch(QDomText textNode);

public:
	/**
	 * @short Create new DomTextRegexpVisitor that splits text nodes on matches of regExp.
	 * @author Rafał 'Vogel' Malinowski
	 * @param regExp regular expression to match on text nodes
	 */
	explicit DomTextRegexpVisitor(QRegExp regExp);
	virtual ~DomTextRegexpVisitor();

	virtual QDomNode visit(QDomText textNode);
	virtual QDomNode beginVisit(QDomElement elementNode);
	virtual QDomNode endVisit(QDomElement elementNode);

	/**
	 * @short Replace matched regular expression with DOM node.
	 * @param document searched DOM document
	 * @param regExp object containing match information
	 * @return DOM nodes replacing matched text
	 */
	virtual QList<QDomNode> matchToDomNodes(QDomDocument document, QRegExp regExp) = 0;

};

/**
 * @}
 */

#endif // DOM_TEXT_REGEXP_VISITOR_H
