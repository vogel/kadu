/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_EXPANDER_H
#define EMOTICON_EXPANDER_H

#include <QtCore/QScopedPointer>

#include "expander/emoticon-path-provider.h"

#include "dom/dom-visitor.h"

class EmoticonPrefixTree;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonExpander
 * @short This DomVisitor expands emoticons in all text nodes in processed DOM document.
 * @author Rafał 'Vogel' Malinowski
 */
class EmoticonExpander : public DomVisitor
{
	EmoticonPrefixTree *Tree;
	QScopedPointer<EmoticonPathProvider> PathProvider;

	/**
	 * @short Insertes emoticon img element at index.
	 * @author Rafał 'Vogel' Malinowski
	 * @param textNode text node to insert emoticon into
	 * @param emoticon emoticon to insert
	 * @param index index of first charater of emoticon in textNode text content
	 * @return new text node created with text after emoticon
	 *
	 * This method splits textNode into two text nodes. First one contains text before emoticon, second one contains
	 * text after emoticon. New img element with emoticon is inserted between these two nodes.
	 *
	 * New text node is returned. Searching for emoticons can be started again for this node.
	 */
	QDomText insertEmoticon(QDomText textNode, const Emoticon &emoticon, int index);

	/**
	 * @short Expands first found emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @param textNode text node to expand first emoticon in
	 * @return new text node created with text after first emoticon
	 *
	 * This node search for first emoticon into text in textNode. If no emoticon is found then null QDomText object
	 * is returned. If emoticon is found then it is expanded and new text node that contains text after this emoticon
	 * is returned. Searching for emoticons can be started again for this node.
	 */
	QDomText expandFirstEmoticon(QDomText textNode);

	virtual QDomNode beginVisit(QDomElement elementNode);
	virtual QDomNode endVisit(QDomElement elementNode);

public:
	/**
	 * @short Create new EmoticonExpander instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param tree emoticon prefix tree used to expand emoticons
	 * @param pathProvider provider that changed emoticon instances into paths to emoticon files
	 *
	 * New object takes ownership of pathProvider object but not on tree object. Tree object must not be destroyed
	 * during EmoticonExpander lifetime.
	 */
	explicit EmoticonExpander(EmoticonPrefixTree *tree, EmoticonPathProvider *pathProvider);
	virtual ~EmoticonExpander();

	/**
	 * @short Expands emoticons in given text node.
	 * @author Rafał 'Vogel' Malinowski
	 * @param textNode text node to expand emoticons in
	 *
	 * This method expands emoticons in given text node. After that new text nodes and elements with img tag names will
	 * be inserted after given textNode. Also text content of current textNode will be cut to first emoticon occurence.
	 */
	virtual QDomNode visit(QDomText textNode);

};

/**
 * @}
 */

#endif // EMOTICON_EXPANDER_H
