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

#ifndef DOM_PROCESSOR_SERVICE_H
#define DOM_PROCESSOR_SERVICE_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "exports.h"

class QDomDocument;

class DomVisitor;

class KADUAPI DomProcessorService: public QObject
{
	Q_OBJECT

	QMap<DomVisitor *, int> Priorities;
	QList<DomVisitor *> Visitors;
	bool VisitorsDirty;

	QList<DomVisitor *> getVisitors();

public:
	explicit DomProcessorService(QObject *parent = 0);
	virtual ~DomProcessorService();

	void registerVisitor(DomVisitor *visitor, int priority);
	void unregisterVisitor(DomVisitor *visitor);

	void process(QDomDocument &domDocument);

	// TODO: remove
	QString process(const QString &html);

};

#endif // DOM_PROCESSOR_SERVICE_H
