/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "dom/dom-visitor-provider-repository.h"
#include "dom/dom-visitor-provider.h"
#include "dom/dom-visitor.h"

#include "dom-processor-service.h"

namespace {

QDomDocument toDomDocument(const QString &xml)
{
	QDomDocument domDocument;
	// force content to be valid HTML with only one root
	domDocument.setContent(QString("<div>%1</div>").arg(xml));

	return domDocument;
}

QString toString(const QDomDocument &domDocument)
{
	if (domDocument.documentElement().childNodes().isEmpty())
		return QString();

	auto result = domDocument.toString(-1).trimmed();
	// remove <div></div>
	Q_ASSERT(result.startsWith(QStringLiteral("<div>")));
	Q_ASSERT(result.endsWith(QStringLiteral("</div>")));
	return result.mid(static_cast<int>(qstrlen("<div>")), result.length() - static_cast<int>(qstrlen("<div></div>")));
}

}

DomProcessorService::DomProcessorService(QObject *parent) :
		QObject(parent)
{
}

DomProcessorService::~DomProcessorService()
{
}

void DomProcessorService::setDomVisitorProviderRepository(DomVisitorProviderRepository *domVisitorProviderRepository)
{
	m_domVisitorProviderRepository = domVisitorProviderRepository;
}

void DomProcessorService::process(QDomDocument &domDocument)
{
	auto visitors = std::vector<const DomVisitor *>();
	for (auto visitorProvider : m_domVisitorProviderRepository)
		if (auto visitor = visitorProvider->provide())
			visitors.push_back(visitor);

	auto domProcessor = DomProcessor{domDocument};
	domProcessor.accept(std::begin(visitors), std::end(visitors));
}

QString DomProcessorService::process(const QString &xml)
{
	auto domDocument = toDomDocument(xml);
	process(domDocument);
	return toString(domDocument);
}

QString DomProcessorService::process(const QString &xml, const DomVisitor &domVisitor)
{
	auto domDocument = toDomDocument(xml);
	auto domProcessor = DomProcessor{domDocument};
	domProcessor.accept(&domVisitor);
	return toString(domDocument);
}

#include "moc_dom-processor-service.cpp"
