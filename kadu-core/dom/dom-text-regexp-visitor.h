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

#ifndef DOM_TEXT_REGEXP_VISITOR_H
#define DOM_TEXT_REGEXP_VISITOR_H

#include <QtCore/QRegExp>

#include "dom/dom-visitor.h"
#include "exports.h"

class QDomDocument;
class QDomNode;

class KADUAPI DomTextRegexpVisitor : public DomVisitor
{
	QRegExp RegExp;

	QDomText expandFirstMatch(QDomText textNode);

public:
	explicit DomTextRegexpVisitor(QRegExp regExp);
	virtual ~DomTextRegexpVisitor();

	virtual void visit(QDomText textNode);
	virtual void beginVisit(QDomElement elementNode);
	virtual void endVisit(QDomElement elementNode);

	virtual QDomNode matchToDomNode(QDomDocument document, QRegExp regExp) = 0;

};

#endif // DOM_TEXT_REGEXP_VISITOR_H
