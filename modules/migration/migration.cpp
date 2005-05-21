
#include "migration.h"

#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <qprocess.h>

#include "misc.h"
#include "message_box.h"
#include "debug.h"

static QString old_ggPath()
{
	char* home;
	struct passwd* pw;
	if ((pw = getpwuid(getuid())))
		home = pw->pw_dir;
	else
		home = getenv("HOME");
	char* config_dir = getenv("CONFIG_DIR");
	if (config_dir == NULL)
		return QString("%1/.gg/").arg(home);
	else
		return QString("%1/%2/.gg/").arg(home).arg(config_dir);
}

extern "C" int migration_init()
{
	kdebugf();
	QString old_path = old_ggPath();
	QString new_path = ggPath("");
	if (!QFile::exists(new_path) && QFile::exists(old_path))
	{
		if (MessageBox::ask(QString("Kadu detected, that you were using EKG, GnuGadu or\n"
					"older version of Kadu before. Would you like to try\n"
					"to import your settings from %1?").arg(old_path)))
		{
			QProcess copy_process(QString("cp"));
			copy_process.addArgument("-r");
			copy_process.addArgument(old_path);
			copy_process.addArgument(new_path);
			if (copy_process.start())
			{
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
					MessageBox::wrn("Error migrating data!");
			}
			else
				MessageBox::wrn("Cannot start migration process!");
		}
	}
	kdebugf2();
	return 0;
}

extern "C" void migration_close()
{
	kdebugf();
	
	kdebugf2();
}

