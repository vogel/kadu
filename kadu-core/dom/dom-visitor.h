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

#ifndef DOM_VISITOR_H
#define DOM_VISITOR_H

class QDomElement;
class QDomNode;
class QDomText;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomVisitor
 * @short Visitor for processing DOM nodes.
 * @author Rafał 'Vogel' Malinowski
 *
 * Accept this visitor in DomProcessor class to process DOM document.
 */
class DomVisitor
{
public:
	virtual ~DomVisitor() {}

	/**
	 * @short Visit current text node.
	 * @author Rafał 'Vogel' Malinowski
	 * @param textNode text node to process
	 * @return new version of visited node
	 *
	 * This method can change textNode and/or document that this node belongs to. It returns new version of visited node.
	 */
	virtual QDomNode visit(QDomText textNode) = 0;

	/**
	 * @short Visit current element before visiting all its subnodes.
	 * @author Rafał 'Vogel' Malinowski
	 * @param elementNode element node to process
	 * @return new version of visited node
	 *
	 * This method can change elementNode and/or document that this node belongs to. This method is called
	 * before subnodes of elementNode are processed. It returns new version of visited node.
	 */
	virtual QDomNode beginVisit(QDomElement elementNode) = 0;

	/**
	 * @short Visit current element after visiting all its subnodes.
	 * @author Rafał 'Vogel' Malinowski
	 * @param elementNode element node to process
	 * @return new version of visited node
	 *
	 * This method can change elementNode and/or document that this node belongs to. This method is called
	 * after subnodes of elementNode are processed. It returns new version of visited node.
	 */
	virtual QDomNode endVisit(QDomElement elementNode) = 0;

};

/**
 * @}
 */

#endif // DOM_VISITOR_H
