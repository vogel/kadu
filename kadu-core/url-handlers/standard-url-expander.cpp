/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtXml/QDomDocument>

#include "standard-url-expander.h"

StandardUrlExpander::StandardUrlExpander(QRegExp regExp) :
		DomTextRegexpVisitor(regExp)
{
}

StandardUrlExpander::~StandardUrlExpander()
{
}

bool StandardUrlExpander::shouldFold(int length)
{
	return Configuration.foldLink() && (length > Configuration.foldLinkThreshold());
}

QString StandardUrlExpander::displayLink(const QString &link)
{
	int partLength = Configuration.foldLinkThreshold() / 2;

	if (shouldFold(link.length()))
		return QString("%1...%2").arg(link.mid(0, partLength)).arg(link.mid(link.length() - partLength, partLength));
	else
		return link;
}

void StandardUrlExpander::setConfiguration(const StandardUrlExpanderConfiguration &configuration)
{
	Configuration = configuration;
}

QList<QDomNode> StandardUrlExpander::matchToDomNodes(QDomDocument document, QRegExp regExp)
{
	QDomElement linkElement = document.createElement("a");

	QString link = regExp.cap();
	QString display = displayLink(link);

	if (link.contains("://"))
		linkElement.setAttribute("href", link);
	else
		linkElement.setAttribute("href", QString("http://%1").arg(link));

	linkElement.setAttribute("title", link);
	if (shouldFold(link.length()))
	{
		linkElement.setAttribute("folded", "1");
		linkElement.setAttribute("displaystr", display);
	}

	linkElement.appendChild(document.createTextNode(display));
	return QList<QDomNode>() << linkElement;
}

#include "moc_standard-url-expander.cpp"
