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

	QList<QStringList> getUinsLists() const;
	int getHistoryEntriesCountPrivate(const QString &filename) const;
	int getHistoryEntriesCount(const QStringList &uins);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

private slots:
	void updateProgressWindow();
	void threadFinished();
	void configurationWindowDestroyed();
	void canceled();

public:
	static HistoryImporter * instance();

public slots:
	void run();
};

#endif // HISTORY_IMPORTER_H
