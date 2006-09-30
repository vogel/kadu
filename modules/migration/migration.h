#ifndef KADU_MIGRATION_H
#define KADU_MIGRATION_H

#include <qdialog.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qpushbutton.h>

/**
 * @defgroup migration Migration
 * @{
 */
class MigrationDialog : public QDialog
{
	Q_OBJECT

	private:
		bool ShouldRestart;
		bool SettingsDirMigrationConfirmed;
		QListView* ProgressListView;
		QLabel* FinishLabel;
		QPushButton* FinishButton;
		QListViewItem* addItem(const QString& text);
		void setItemComplete(QListViewItem* item, const QString& text,
			const QString& details, bool restart = false);
		QString old_ggPath();
		bool settingsDirMigrationNeeded();
		void settingsDirMigration();
		void xmlConfigFileMigration(const QString& config_name);
		bool xmlConfigFilesMigrationNeeded();
		void xmlConfigFilesMigration();
		bool xmlUserListMigrationNeeded();
		void xmlUserListMigration();
		bool xmlIgnoredListMigrationNeeded();
		void xmlIgnoredListMigration();

	private slots:
		void finishButtonClicked();

	public:
		MigrationDialog();
		~MigrationDialog();
		void migrate();
};

/** @} */

#endif
