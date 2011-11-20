/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef HISTORY_IMPORTER_MANAGER_H
#define HISTORY_IMPORTER_MANAGER_H

#include <QtCore/QObject>

#include "history_migration_exports.h"

class HistoryImporter;

class HISTORY_MIGRATION_API HistoryImporterManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(HistoryImporterManager)

	static HistoryImporterManager *Instance;

	QList<HistoryImporter *> Importers;

	HistoryImporterManager();
	~HistoryImporterManager();

private slots:
	void importerDestroyed(QObject *importer);

public:
	static void createInstance();
	static void destroyInstance();

	static HistoryImporterManager * instance() { return Instance; }

	bool containsImporter(const QString &path);

	void addImporter(HistoryImporter *importer);
	void removeImporter(HistoryImporter *importer);

};

#endif // HISTORY_IMPORTER_MANAGER_H
