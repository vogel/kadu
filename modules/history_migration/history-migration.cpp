/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtGui/QProgressDialog>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "misc/path-conversion.h"
#include "modules/history/history.h"
#include "debug.h"

#include "history-import-thread.h"

#include "history-migration.h"

extern "C" KADU_EXPORT int history_migration_init(bool firstLoad)
{
	kdebugf();

	HistoryImporter *hi = HistoryImporter::instance();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/history-migration.ui"));

	bool imported = config_file.readBoolEntry("History", "Imported_from_0.6.5", false);

	if (!imported && firstLoad && QFile::exists(profilePath("history")))
		hi->run();

	return 0;
}

extern "C" KADU_EXPORT void history_migration_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/history-migration.ui"));

	HistoryImporter::instance()->canceled();
}

HistoryImporter * HistoryImporter::Instance = 0;

HistoryImporter * HistoryImporter::instance()
{
	if (!Instance)
		Instance = new HistoryImporter();

	return Instance;
}

HistoryImporter::HistoryImporter() :
		ConfigurationWindow(0), Thread(0)
{
	kdebugf();

	MainConfigurationWindow::registerUiHandler(this);
}

HistoryImporter::~HistoryImporter()
{
	MainConfigurationWindow::unregisterUiHandler(this);

	Instance = 0;

	kdebugf();
}

void HistoryImporter::run()
{
	kdebugf();

	if (Thread)
		return;

	if (config_file.readBoolEntry("History", "Imported_from_0.6.5", false))
		return;

	Account gaduAccount = Account::null;
	foreach (Account account, AccountManager::instance()->items())
		if (account.protocolHandler() && account.protocolHandler()->protocolFactory()
			&& account.protocolHandler()->protocolFactory()->name() == "gadu")
		{
			gaduAccount = account;
			break;
		}

	if (gaduAccount.isNull() || !History::instance()->currentStorage())
		return;

	QList<UinsList> uinsLists = HistoryMigrationHelper::getUinsLists();
	int totalEntries = 0;

	foreach (const UinsList &uinsList, uinsLists)
		totalEntries += HistoryMigrationHelper::getHistoryEntriesCount(uinsList);

	if (0 == totalEntries)
		return;

	if (ConfigurationWindow)
		ConfigurationWindow->widget()->widgetById("history-migration/import")->setVisible(false);

	ProgressDialog = new QProgressDialog(qApp->translate("HistoryMigration", "Migrating old history: %1 of %2").arg(0).arg(totalEntries),
	"", 0, totalEntries, Core::instance()->kaduWindow());
	ProgressDialog->setCancelButton(0);
	ProgressDialog->setWindowModality(Qt::NonModal);
	ProgressDialog->setAutoClose(false);
	connect(ProgressDialog, SIGNAL(canceled()), this, SLOT(canceled()));

	Thread = new HistoryImportThread(gaduAccount, uinsLists, totalEntries);
	connect(Thread, SIGNAL(finished()), this, SLOT(threadFinished()));

	QTimer *updateProgressBar = new QTimer(this);
	updateProgressBar->setSingleShot(false);
	updateProgressBar->setInterval(200);
	connect(updateProgressBar, SIGNAL(timeout()), this, SLOT(updateProgressWindow()));

	Thread->start();
	ProgressDialog->show();
	updateProgressBar->start();
}

void HistoryImporter::updateProgressWindow()
{
	if (ProgressDialog)
	{
		ProgressDialog->setValue(Thread->importedEntries());
		ProgressDialog->setLabelText(qApp->translate("HistoryMigration", "Migrating old history: %1 of %2")
				.arg(Thread->importedEntries())
				.arg(ProgressDialog->maximum()));
	}
}

void HistoryImporter::threadFinished()
{
	config_file.writeEntry("History", "Imported_from_0.6.5", true);

	delete ProgressDialog;
	ProgressDialog = 0;

	deleteLater();
}

void HistoryImporter::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigurationWindow = mainConfigurationWindow;
	connect(ConfigurationWindow, SIGNAL(destroyed()), this, SLOT(configurationWindowDestroyed()));

	QWidget *importButton = mainConfigurationWindow->widget()->widgetById("history-migration/import");
	importButton->setVisible(!config_file.readBoolEntry("History", "Imported_from_0.6.5", false));

	connect(importButton, SIGNAL(clicked()), this, SLOT(run()));
}

void HistoryImporter::configurationWindowDestroyed()
{
	ConfigurationWindow = 0;
}

void HistoryImporter::canceled()
{
	if (Thread)
		Thread->cancel();

	deleteLater();
}
