/*
 * %kadu copyright begin%
 * %kadu copyright end%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Michał Obrembski (byku@byku.com.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtXml/QDomDocument>

#include "video-expander.h"

VideoExpander::VideoExpander() :
		DomTextRegexpVisitor(QRegExp("http://www.youtube.com/watch(.*)&?"))
{
}

VideoExpander::~VideoExpander()
{
}

QList<QDomNode> VideoExpander::matchToDomNodes(QDomDocument document, QRegExp regExp)
{
	QDomElement objectElement = document.createElement("object");
	objectElement.setAttribute("width", "33%");
	objectElement.setAttribute("height", "33%");

	QDomElement embedElement = document.createElement("embed");
	embedElement.setAttribute("src", QString("http://www.youtube.com/v/%1&autoplay=0").arg(regExp.cap(1)));
	embedElement.setAttribute("type", "application/x-shockwave-flash");
	embedElement.setAttribute("allowscriptaccess", "always");
	embedElement.setAttribute("allowfullscreen", "true");
	embedElement.setAttribute("width", "33%");
	embedElement.setAttribute("height", "33%");

	objectElement.appendChild(embedElement);

	QDomText textElement = document.createTextNode(regExp.cap());

	return QList<QDomNode>() << objectElement << textElement;
}
