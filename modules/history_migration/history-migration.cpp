/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtGui/QProgressDialog>

#include "accounts/account-manager.h"
#include "chat/message/message.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-box.h"
#include "misc/misc.h"
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

	if (!imported && firstLoad && QFile::exists(ggPath("history")))
		hi->run();

	return 0;
}

extern "C" KADU_EXPORT void history_migration_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/history-migration.ui"));
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

	Account *gaduAccount = 0;
	foreach (Account *account, AccountManager::instance()->accounts())
		if (account->protocol() && account->protocol()->protocolFactory()
			&& account->protocol()->protocolFactory()->name() == "gadu")
		{
			gaduAccount = account;
			break;
		}

	if (!gaduAccount || !History::instance()->currentStorage())
		return;

	QList<QStringList> uinsLists = getUinsLists();
	int totalEntries = 0;

	foreach (QStringList uinsList, uinsLists)
		totalEntries += getHistoryEntriesCount(uinsList);

	if (0 == totalEntries)
		return;

	if (!MessageBox::ask(qApp->translate("HistoryMigration", "%1 history entries found. Do you want to import them?").arg(totalEntries)))
		return;
	
	if (ConfigurationWindow)
		ConfigurationWindow->widget()->widgetById("history-migration/import")->setVisible(false);

	ProgressDialog = new QProgressDialog(qApp->translate("HistoryMigration", "Migrating old history: %1 of %2").arg(0).arg(totalEntries),
	qApp->translate("HistoryMigration", "Cancel"), 0, totalEntries, Core::instance()->kaduWindow());
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

QList<QStringList> HistoryImporter::getUinsLists() const
{
	kdebugf();
	QList<QStringList> entries;
	QDir dir(ggPath("history/"), "*.idx");
	QStringList struins;
	QStringList uins;

	foreach (QString entry, dir.entryList())
	{
		struins = entry.remove(QRegExp(".idx$")).split("_", QString::SkipEmptyParts);
		uins.clear();
		if (struins[0] != "sms")
			foreach (const QString &struin, struins)
				uins.append(struin);
		entries.append(uins);
	}

	kdebugf2();
	return entries;
}

int HistoryImporter::getHistoryEntriesCountPrivate(const QString &filename) const
{
	kdebugf();

	int lines;
	QFile f;
	QString path = ggPath("history/");
	QByteArray buffer;

	f.setFileName(path + filename + ".idx");
	if (!f.open(QIODevice::ReadOnly))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", qPrintable(filename));
		return 0;
	}
	lines = f.size() / sizeof(int);

	f.close();

	kdebugmf(KDEBUG_INFO, "%d lines\n", lines);
	return lines;
}

int HistoryImporter::getHistoryEntriesCount(const QStringList &uins)
{
	kdebugf();
	int ret = getHistoryEntriesCountPrivate(HistoryImportThread::getFileNameByUinsList(uins));
	kdebugf2();
	return ret;
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
