/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "dom/dom-visitor-provider.h"
#include "dom/dom-visitor.h"

#include "dom-processor-service.h"

DomProcessorService::DomProcessorService(QObject *parent) :
		QObject(parent), VisitorProvidersDirty(false)
{
}

DomProcessorService::~DomProcessorService()
{
}

QList<DomVisitorProvider *> DomProcessorService::getVisitorProviders()
{
	if (!VisitorProvidersDirty)
		return VisitorProviders;

	VisitorProviders.clear();

	QMultiMap<int, DomVisitorProvider *> inverted;
	foreach (DomVisitorProvider *visitorProvider, Priorities.keys())
		inverted.insert(Priorities.value(visitorProvider), visitorProvider);

	foreach (int priority, inverted.keys())
		VisitorProviders.append(inverted.values(priority));

	return VisitorProviders;
}

void DomProcessorService::registerVisitorProvider(DomVisitorProvider *visitorProvider, int priority)
{
	if (Priorities.contains(visitorProvider))
		return;

	Priorities.insert(visitorProvider, priority);
	VisitorProvidersDirty = true;
}

void DomProcessorService::unregisterVisitorProvider(DomVisitorProvider *visitorProvider)
{
	if (0 < Priorities.remove(visitorProvider))
		VisitorProvidersDirty = true;
}


void DomProcessorService::process(QDomDocument &domDocument)
{
	QList<DomVisitorProvider *> visitorProviders = getVisitorProviders();
	if (visitorProviders.isEmpty())
		return;

	DomProcessor domProcessor(domDocument);
	foreach (DomVisitorProvider *visitorProvider, visitorProviders)
	{
		DomVisitor *visitor = visitorProvider->provide();
		if (visitor)
			domProcessor.accept(visitor);
	}
}

QString DomProcessorService::process(const QString &xml)
{
	QDomDocument domDocument;
	// force content to be valid HTML with only one root
	domDocument.setContent(QString("<div>%1</div>").arg(xml));

	process(domDocument);

	if (domDocument.documentElement().childNodes().isEmpty())
		return QString();

	QString result = domDocument.toString(0).trimmed();
	// remove <div></div>
	Q_ASSERT(result.startsWith(QLatin1String("<div>")));
	Q_ASSERT(result.endsWith(QLatin1String("</div>")));
	return result.mid(static_cast<int>(qstrlen("<div>")), result.length() - static_cast<int>(qstrlen("<div></div>")));
}

#include "moc_dom-processor-service.cpp"
