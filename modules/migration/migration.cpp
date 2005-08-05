
#include "migration.h"

#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <qprocess.h>
#include <qlayout.h>
#include <qgroupbox.h>

#include "misc.h"
#include "message_box.h"
#include "debug.h"
#include "userlist.h"
#include "kadu.h"
#include "ignore.h"

MigrationDialog::MigrationDialog()
	: QDialog(NULL, NULL, true), ShouldRestart(false), SettingsDirMigrationConfirmed(false)
{
	resize(500, 300);
	setCaption(tr("Migration"));
	QVBoxLayout* layout = new QVBoxLayout(this);
	ProgressListView = new QListView(this);
	ProgressListView->addColumn(tr("Migration steps"));
	//ProgressListView->setSorting(-1);
	ProgressListView->setRootIsDecorated(true);
	layout->addWidget(ProgressListView);
	QGroupBox* bottom_box = new QGroupBox(2, Qt::Horizontal, this);
	FinishLabel = new QLabel(tr("Migration in progress ..."), bottom_box);
	FinishButton = new QPushButton(tr("&Finish"), bottom_box);
	connect(FinishButton, SIGNAL(clicked()), this, SLOT(finishButtonClicked()));
	layout->addWidget(bottom_box);
}

MigrationDialog::~MigrationDialog()
{
	disconnect(FinishButton, SIGNAL(clicked()), this, SLOT(finishButtonClicked()));
}

QListViewItem* MigrationDialog::addItem(const QString& text)
{
	QListViewItem* item = new QListViewItem(ProgressListView, ProgressListView->lastItem());
	item->setText(0, text);
	return item;
}

void MigrationDialog::setItemComplete(QListViewItem* item, const QString& text,
	const QString& details, bool restart)
{
	item->setText(0, text);
	QListViewItem* sub_item = new QListViewItem(item, details);
	sub_item->setMultiLinesEnabled(true);
	if (restart)
		ShouldRestart = true;
}

QString MigrationDialog::old_ggPath()
{
	kdebugf();
	char* home;
	struct passwd* pw;
	if ((pw = getpwuid(getuid())))
		home = pw->pw_dir;
	else
		home = getenv("HOME");
	char* config_dir = getenv("CONFIG_DIR");
	QString path;
	if (config_dir == NULL)
		path = QString("%1/.gg/").arg(home);
	else
		path = QString("%1/%2/gg/").arg(home).arg(config_dir);
	return path;
}

bool MigrationDialog::settingsDirMigrationNeeded()
{
	kdebugf();
	QString old_path = old_ggPath();
	QString new_path = ggPath("");
	new_path.truncate(new_path.length() - 1); // obetnij konczacy /
	kdebug("old_path: %s\n", old_path.local8Bit().data());
	kdebug("new_path: %s\n", new_path.local8Bit().data());
	if (QFile::exists(new_path) || !QFile::exists(old_path))
	{
		kdebugf2();
		return false;
	}
	if (SettingsDirMigrationConfirmed ||
		MessageBox::ask(tr("Kadu detected, that you were using EKG, GnuGadu or\n"
				"older version of Kadu before. Would you like to try\n"
				"to import your settings from %1?").arg(old_path)))
	{
		SettingsDirMigrationConfirmed = true;
		kdebugf2();
		return true;
	}
	kdebugf2();
	return false;
}

void MigrationDialog::settingsDirMigration()
{
	kdebugf();
	QString old_path = old_ggPath();
	QString new_path = ggPath("");
	new_path.truncate(new_path.length() - 1); // obetnij konczacy /
	kdebug("old_path: %s\n", old_path.local8Bit().data());
	kdebug("new_path: %s\n", new_path.local8Bit().data());
	if (!settingsDirMigrationNeeded())
	{
		kdebugf2();
		return;
	}
	QListViewItem* item = addItem(tr("Step 1: Migrating settings to kadu directory"));
	kdebug("creating process: cp\n");
	QProcess copy_process(QString("cp"));
	kdebug("adding argument: -r\n");
	copy_process.addArgument("-r");
	kdebug("adding argument: %s\n", old_path.local8Bit().data());
	copy_process.addArgument(old_path);
	kdebug("adding argument: %s\n", new_path.local8Bit().data());
	copy_process.addArgument(new_path);
	kdebug("starting process\n");
	if (copy_process.start())
	{
		kdebug("process started, waiting while it is running\n");
		while (copy_process.isRunning()) { };
		if (copy_process.normalExit() && copy_process.exitStatus() == 0)
		{
			setItemComplete(item,
				tr("Step 1: Settings migrated to kadu directory"),
				tr("Migration process completed. You can remove\n%1"
				"directory\n(backup will be a good idea) or leave it"
				" for other applications.").arg(old_path), true);
			kdebugf2();
			return;
		}
		else
		{
			kdebug("error migrating data. exit status: %i\n",
				copy_process.exitStatus());
			MessageBox::wrn(tr("Error migrating data!"));
		}
	}
	else
	{
		kdebug("cannot start migration process\n");
		MessageBox::wrn(tr("Cannot start migration process!"));
	}
	kdebugf2();
	return;
}

bool MigrationDialog::xmlUserListMigrationNeeded()
{
	kdebugf();
	QString userlist_path = ggPath("userlist");
	QString userattribs_path = ggPath("userattribs");
	if (xml_config_file->rootElement().elementsByTagName("Contacts").length() == 0 &&
		QFile::exists(userlist_path) && QFile::exists(userattribs_path))
	{
		kdebugf2();
		return true;
	}
	kdebugf2();
	return false;
}

void MigrationDialog::xmlUserListMigration()
{
	kdebugf();
	QString userlist_path = ggPath("userlist");
	QString userattribs_path = ggPath("userattribs");
	if (xmlUserListMigrationNeeded())
	{
		QListViewItem* item = addItem(
			tr("Step 2: Migrating user list to kadu.conf.xml"));
		userlist.readFromFile();
		userlist.writeToConfig();
		xml_config_file->sync();
		kadu->setActiveGroup("");
		setItemComplete(item,
			tr("Step 2: User list migrated to kadu.conf.xml"),
			tr("Contact list migrated to kadu.conf.xml."
			"You can remove\n%1 and\n%2 now\n(backup will be a good idea).")
			.arg(userlist_path).arg(userattribs_path));
	}
	kdebugf2();
}

bool MigrationDialog::xmlIgnoredListMigrationNeeded()
{
	kdebugf();
	QString ignored_path = ggPath("ignore");
	kdebug("ignored_path: %s\n", ignored_path.local8Bit().data());
	if (xml_config_file->rootElement().elementsByTagName("Ignored").length() == 0 &&
		QFile::exists(ignored_path))
	{
		kdebugf2();
		return true;
	}
	kdebugf2();
	return false;
}

void MigrationDialog::xmlIgnoredListMigration()
{
	kdebugf();
	QString ignored_path = ggPath("ignore");
	kdebug("ignored_path: %s\n", ignored_path.local8Bit().data());
	if (xmlIgnoredListMigrationNeeded())
	{
		kdebug("migrating ignored list\n");
		QListViewItem* item = addItem(tr("Step 3: Migrating ignored list to kadu.conf.xml"));
		QFile f(ignored_path);
		if (!f.open(IO_ReadOnly))
		{
			kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "can't open ignore file!\n");
			return;
		}
		QTextStream stream(&f);
		QString line;
		kdebug("creating Ignored element\n");
		QDomElement ignored_elem =
			xml_config_file->createElement(xml_config_file->rootElement(), "Ignored");
		kdebug("begin of ignored file loop\n");
		while ((line = stream.readLine()) != QString::null)
		{
			kdebug("ignored file line: %s\n", line.local8Bit().data());
			UinsList uins;
			QStringList list = QStringList::split(";", line);
			QDomElement ignored_grp_elem =
				xml_config_file->createElement(ignored_elem, "IgnoredGroup");
			kdebug("begin of ignored group loop\n");
			CONST_FOREACH(strUin, list)
			{
				kdebug("ignored uin: %s\n", (*strUin).local8Bit().data());
				QDomElement ignored_contact_elem =
					xml_config_file->createElement(ignored_grp_elem,
						"IgnoredContact");
				ignored_contact_elem.setAttribute("uin", (*strUin));
			}
			kdebug("end of ignored group loop\n");
		}
		kdebug("end of ignored file loop\n");
		f.close();
		xml_config_file->sync();
		readIgnored();
		setItemComplete(item, tr("Step 3: Ignored list migrated to kadu.conf.xml"),
			tr("Ignored contact list migrated to kadu.conf.xml.\n"
			"You can remove %1 now\n(backup will be a good idea).")
			.arg(ignored_path));
	}
	kdebugf2();
}

void MigrationDialog::xmlConfigFileMigration(const QString& config_name)
{
	kdebugf();
	QString config_path = ggPath(config_name);
	kdebug("config_path: %s\n", config_path.local8Bit().data());
	QDomElement root_elem = xml_config_file->rootElement();
	QFile file(config_path);
	QString line;
#if QT_VERSION < 0x030100
	QRegExp newLine("\\\\n");
#endif
	if (file.open(IO_ReadOnly))
	{
		QTextStream stream(&file);
		stream.setCodec(codec_latin2);
		QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
		QDomElement conf_elem = xml_config_file->createElement(deprecated_elem, "ConfigFile");
		conf_elem.setAttribute("name", config_name);
		QDomElement group_elem;
		while (!stream.atEnd())
		{
			line = stream.readLine();
			line.stripWhiteSpace();
			if (line.startsWith("[") && line.endsWith("]"))
			{
				QString name = line.mid(1, line.length() - 2).stripWhiteSpace();
				kdebug("group: %s\n", name.local8Bit().data());
				group_elem = xml_config_file->createElement(conf_elem, "Group");
				group_elem.setAttribute("name", name);
			}
			else if (!group_elem.isNull())
			{
				kdebug("line: %s\n", line.local8Bit().data());
				QString name = line.section('=', 0, 0);
#if QT_VERSION < 0x030100
				//kilka razy wolniejsze...
				QString value = line.right(line.length()-name.length()-1).replace(newLine, "\n");
#else
				QString value = line.right(line.length()-name.length()-1).replace("\\n", "\n");
#endif
				name = name.stripWhiteSpace();
				if (line.contains('=') >= 1 && !name.isEmpty() && !value.isEmpty())
				{
					kdebug("entry: %s=%s\n", name.local8Bit().data(),
						value.local8Bit().data());
					QDomElement entry_elem =
						xml_config_file->createElement(group_elem, "Entry");
					entry_elem.setAttribute("name", name);
					entry_elem.setAttribute("value", value);
				}
			}
		}
		file.close();
	}
	kdebugf2();
}

bool MigrationDialog::xmlConfigFilesMigrationNeeded()
{
	kdebugf();
	QString config_path = ggPath("kadu.conf");
	kdebug("config_path: %s\n", config_path.local8Bit().data());
	QDomElement root_elem = xml_config_file->rootElement();
	if (!QFile::exists(config_path) ||
		!xml_config_file->findElement(root_elem, "Deprecated").isNull())
	{
		kdebugf2();
		return false;
	}
	kdebugf2();
	return true;
}

void MigrationDialog::xmlConfigFilesMigration()
{
	kdebugf();
	QString config_path = ggPath("kadu.conf");
	kdebug("config_path: %s\n", config_path.local8Bit().data());
	QDomElement root_elem = xml_config_file->rootElement();
	if (!xmlConfigFilesMigrationNeeded())
	{
		kdebugf2();
		return;
	}
	QListViewItem* item = addItem(tr("Step 4: Migrating config files to kadu.conf.xml"));
	QDir dir(ggPath(""));
	dir.setNameFilter("*.conf");
	for (int i = 0; i < dir.count(); i++)
		xmlConfigFileMigration(dir[i]);
	xml_config_file->sync();
	setItemComplete(item, tr("Step 4: Config files migrated to kadu.conf.xml"),
		tr("Configuration files migrated to kadu.conf.xml.\n"
		"You can remove following files now:\n%1\n(backup will be a good idea).")
		.arg(dir.entryList().join(",")), true);
	kdebugf2();
}

void MigrationDialog::migrate()
{
	kdebugf();
	if (settingsDirMigrationNeeded() || xmlConfigFilesMigrationNeeded() ||
		xmlUserListMigrationNeeded() || xmlIgnoredListMigrationNeeded())
	{
		show();
		settingsDirMigration();
		xmlConfigFilesMigration();
		xmlUserListMigration();
		xmlIgnoredListMigration();
		if (ShouldRestart)
			FinishLabel->setText(
				tr("Migration complete. Kadu will be closed now.\n"
				"Please click Finish and than run Kadu again."));
		else
			FinishLabel->setText(tr("Migration complete."));
		exec();
		if (ShouldRestart)
			_exit(0);
	}
	kdebugf2();
}

void MigrationDialog::finishButtonClicked()
{
	accept();
}

extern "C" int migration_init()
{
	kdebugf();
	MigrationDialog* migration_dialog = new MigrationDialog();
	migration_dialog->migrate();
	delete migration_dialog;
	kdebugf2();
	return 0;
}

extern "C" void migration_close()
{
	kdebugf();
	
	kdebugf2();
}

