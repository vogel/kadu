
#include "migration.h"

#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <qprocess.h>

#include "misc.h"
#include "message_box.h"
#include "debug.h"
#include "userlist.h"

static QString old_ggPath()
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
		path = QString("%1/%2/.gg/").arg(home).arg(config_dir);
	return path;
}

static void settingsDirMigration()
{
	kdebugf();
	QString old_path = old_ggPath();
	QString new_path = ggPath("");
	new_path.truncate(new_path.length() - 1); // obetnij konczacy /
	if (QFile::exists(new_path) || !QFile::exists(old_path))
	{
		kdebugf2();
		return;
	}
	if (MessageBox::ask(QString("Kadu detected, that you were using EKG, GnuGadu or\n"
				"older version of Kadu before. Would you like to try\n"
				"to import your settings from %1?").arg(old_path)))
	{
		kdebug("creating process: cp");
		QProcess copy_process(QString("cp"));
		kdebug("adding argument: -r");
		copy_process.addArgument("-r");
		kdebug("adding argument: %s", old_path.local8Bit().data());
		copy_process.addArgument(old_path);
		kdebug("adding argument: %s", new_path.local8Bit().data());
		copy_process.addArgument(new_path);
		kdebug("starting process");
		if (copy_process.start())
		{
			kdebug("process started, waiting while it is running");
			MessageBox::status("Migrating data ...");
			while (copy_process.isRunning()) { };
			MessageBox::close("Migrating data ...");
			if (copy_process.normalExit() && copy_process.exitStatus() == 0)
			{
				MessageBox::msg(QString("Migration process completed. You can remove %1 directory\n"
							"(backup will be a good idea) or leave it for other applications.\n"
							"Kadu will be closed now. Please click OK and than run Kadu again.").arg(old_path),
							true);
				_exit(0);
			}
			else
			{
				kdebug("error migrating data. exit status: %i",
					copy_process.exitStatus());
				MessageBox::wrn("Error migrating data!");
			}
		}
		else
		{
			kdebug("cannot start migration process");
			MessageBox::wrn("Cannot start migration process!");
		}
	}
	kdebugf2();
}

static void xmlUserListMigration()
{
	kdebugf();
	QString userlist_path = ggPath("userlist");
	QString userattribs_path = ggPath("userattribs");
	if (xml_config_file->rootElement().elementsByTagName("Contacts").length() == 0 &&
		QFile::exists(userlist_path) && QFile::exists(userattribs_path))
	{
		userlist.readFromFile();
		userlist.writeToConfig();
		xml_config_file->sync();
		MessageBox::msg(QString("Contact list migrated to kadu.conf.xml.\n"
			"You can remove %1\n"
			"and %2 now\n"
			"(backup will be a good idea).\n").arg(userlist_path).arg(userattribs_path));
	}
	kdebugf2();
}

extern "C" int migration_init()
{
	kdebugf();
	settingsDirMigration();
	xmlUserListMigration();
	kdebugf2();
	return 0;
}

extern "C" void migration_close()
{
	kdebugf();
	
	kdebugf2();
}

