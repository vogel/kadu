
#include "migration.h"

#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <qprocess.h>

#include "misc.h"
#include "message_box.h"
#include "debug.h"
#include "userlist.h"
#include "kadu.h"
#include "ignore.h"

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
		path = QString("%1/%2/gg/").arg(home).arg(config_dir);
	return path;
}

static void settingsDirMigration()
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
		return;
	}
	if (MessageBox::ask(QString("Kadu detected, that you were using EKG, GnuGadu or\n"
				"older version of Kadu before. Would you like to try\n"
				"to import your settings from %1?").arg(old_path)))
	{
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
				kdebug("error migrating data. exit status: %i\n",
					copy_process.exitStatus());
				MessageBox::wrn("Error migrating data!");
			}
		}
		else
		{
			kdebug("cannot start migration process\n");
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
		kadu->setActiveGroup("");
		MessageBox::msg(QString("Contact list migrated to kadu.conf.xml.\n"
			"You can remove %1\n"
			"and %2 now\n"
			"(backup will be a good idea).\n").arg(userlist_path).arg(userattribs_path));
	}
	kdebugf2();
}

static void xmlIgnoredListMigration()
{
	kdebugf();
	QString ignored_path = ggPath("ignore");
	kdebug("ignored_path: %s\n", ignored_path.local8Bit().data());
	if (xml_config_file->rootElement().elementsByTagName("Ignored").length() == 0 &&
		QFile::exists(ignored_path))
	{
		kdebug("migrating ignored list\n");
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
		MessageBox::msg(QString("Ignored contact list migrated to kadu.conf.xml.\n"
			"You can remove %1 now\n"
			"(backup will be a good idea).\n").arg(ignored_path));	
	}
	kdebugf2();
}

static void xmlConfigFileMigration()
{
	kdebugf();
	QString config_path = ggPath("kadu.conf");
	kdebug("config_path: %s\n", config_path.local8Bit().data());
	QDomElement root_elem = xml_config_file->rootElement();
	if (!QFile::exists(config_path) ||
		!xml_config_file->findElement(root_elem, "Deprecated").isNull())
	{
		kdebugf2();
		return;
	}
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
		QDomElement conf_elem = xml_config_file->accessElement(deprecated_elem, "ConfigFile");
		conf_elem.setAttribute("name", "kadu.conf");
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
		xml_config_file->sync();
		MessageBox::msg(QString("Configuration file migrated to kadu.conf.xml.\n"
			"You can remove %1 now\n"
			"(backup will be a good idea).\n").arg(config_path));		
	}
	kdebugf2();
}

extern "C" int migration_init()
{
	kdebugf();
	settingsDirMigration();
	xmlUserListMigration();
	xmlIgnoredListMigration();
	xmlConfigFileMigration();
	kdebugf2();
	return 0;
}

extern "C" void migration_close()
{
	kdebugf();
	
	kdebugf2();
}

