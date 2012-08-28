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

#include <QtXml/QDomDocument>

#include "dom/dom-processor.h"
#include "dom/dom-visitor.h"

#include "dom-processor-service.h"

DomProcessorService::DomProcessorService(QObject *parent) :
		QObject(parent), VisitorsDirty(false)
{
}

DomProcessorService::~DomProcessorService()
{
}

QList<DomVisitor *> DomProcessorService::getVisitors()
{
	if (!VisitorsDirty)
		return Visitors;

	Visitors.clear();

	QMultiMap<int, DomVisitor *> inverted;
	foreach (DomVisitor *visitor, Priorities.keys())
		inverted.insert(Priorities.value(visitor), visitor);

	foreach (int priority, inverted.keys())
		Visitors.append(inverted.values(priority));

	return Visitors;
}

void DomProcessorService::registerVisitor(DomVisitor *visitor, int priority)
{
	if (Priorities.contains(visitor))
		return;

	Priorities.insert(visitor, priority);
	VisitorsDirty = true;
}

void DomProcessorService::unregisterVisitor(DomVisitor *visitor)
{
	if (0 < Priorities.remove(visitor))
		VisitorsDirty = true;
}


void DomProcessorService::process(QDomDocument &domDocument)
{
	QList<DomVisitor *> visitors = getVisitors();
	if (visitors.isEmpty())
		return;

	DomProcessor domProcessor(domDocument);
	foreach (DomVisitor *visitor, visitors)
		domProcessor.accept(visitor);
}

QString DomProcessorService::process(const QString &html)
{
	QDomDocument domDocument;
	// force content to be valid HTML with only one root
	domDocument.setContent(QString("<div>%1</div>").arg(html));

	process(domDocument);

	if (domDocument.documentElement().childNodes().isEmpty())
		return QString();

	QString result = domDocument.toString(0).trimmed();
	// remove <div></div>
	Q_ASSERT(result.startsWith(QLatin1String("<div>")));
	Q_ASSERT(result.endsWith(QLatin1String("</div>")));
	return result.mid(qstrlen("<div>"), result.length() - qstrlen("<div></div>"));
}
