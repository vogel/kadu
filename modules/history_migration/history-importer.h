/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef HISTORY_IMPORTER_H
#define HISTORY_IMPORTER_H

#include "gui/windows/main-configuration-window.h"

class QProgressDialog;

class HistoryImportThread;

class HistoryImporter : public ConfigurationUiHandler
{
	Q_OBJECT
	Q_DISABLE_COPY(HistoryImporter)

	static HistoryImporter *Instance;

	MainConfigurationWindow *ConfigurationWindow;
	HistoryImportThread *Thread;
	QProgressDialog *ProgressDialog;

	HistoryImporter();
	virtual ~HistoryImporter();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

private slots:
	void updateProgressWindow();
	void threadFinished();
	void configurationWindowDestroyed();

public:
	static HistoryImporter * instance();

public slots:
	void run();
	void canceled();

};

#endif // HISTORY_IMPORTER_H
