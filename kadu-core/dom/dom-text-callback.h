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

#ifndef DOM_TEXT_CALLBACK_H
#define DOM_TEXT_CALLBACK_H

class QDomText;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomTextCallback
 * @short Callback for processing text node.
 * @author Rafał 'Vogel' Malinowski
 *
 * Implementations of this interface can be set in DomProcessor to handle processing of text DOM nodes.
 */
class DomTextCallback
{
public:
	/**
	 * @short Process current text node.
	 * @author Rafał 'Vogel' Malinowski
	 * @param textNode text node to process
	 *
	 * This method can change textNode and/or document that this node belongs to.
	 */
	virtual void processDomText(QDomText textNode) = 0;

};

/**
 * @}
 */

#endif // DOM_TEXT_CALLBACK_H
