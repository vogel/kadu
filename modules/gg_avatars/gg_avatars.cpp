/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QBuffer>
#include <QtCore/QFileInfo>
#include <QtNetwork/QHttp>

#include "gg_avatars.h"

#include "gadu.h"
#include "kadu.h"
#include "debug.h"
#include "modules.h"
#include "userbox.h"
#include "kadu_parser.h"
#include "config_file.h"
#include "icons_manager.h"
#include "userlistelement.h"

GaduAvatars *gaduAvatars = 0;
const QString suffix =
#ifdef Q_OS_WIN
	"avatars\\";
#else
	"avatars/";
#endif

extern "C" KADU_EXPORT int gg_avatars_init()
{
	kdebugf();

	gaduAvatars = new GaduAvatars();

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void gg_avatars_close()
{
	kdebugf();

	delete gaduAvatars;
	gaduAvatars = NULL;

	kdebugf2();
}

QString get_avatar_url(const UserListElement &ule)
{
	if (ule.usesProtocol("Gadu"))
		return gaduAvatars->getAvatar(ule.ID("Gadu").toInt(), MODE_SMALL);
	else
		return QString();
}

QString get_avatar(const UserListElement &ule)
{
	QString avatar = get_avatar_url(ule);
	if (!avatar.isEmpty())
	{
#ifdef Q_OS_WIN
		/* un-escape the spaces in filename */
		avatar = avatar.replace("\\ ", " ");
		avatar = "<img src=\"" + avatar + "\"/>";
#else
		avatar = "<img src=\"file://" + avatar + "\"/>";
#endif
	}
	return avatar;
}

QString get_big_avatar_url(const UserListElement &ule)
{
	if (ule.usesProtocol("Gadu"))
		return gaduAvatars->getAvatar(ule.ID("Gadu").toInt(), MODE_BIG);
	else
		return QString();
}

QString get_big_avatar(const UserListElement &ule)
{
	QString avatar = get_big_avatar_url(ule);
	if (!avatar.isEmpty())
	{
#ifdef Q_OS_WIN
		/* un-escape the spaces in filename */
		avatar = avatar.replace("\\ ", " ");
		avatar = "<img src=\"" + avatar + "\"/>";
#else
		avatar = "<img src=\"file://" + avatar + "\"/>";
#endif

	}

	return avatar;
}

GaduAvatars::GaduAvatars()
{
	linkDownloader = new QHttp("api.gadu-gadu.pl");
	connect(linkDownloader, SIGNAL(requestFinished(int, bool)), this, SLOT(gotResponse(int, bool)));

	fileDownloader = new QHttp();
	connect(fileDownloader, SIGNAL(requestFinished(int, bool)), this, SLOT(fileDownloaded(int, bool)));

	KaduParser::registerTag("avatar", &get_avatar);
	KaduParser::registerTag("avatar_url", &get_avatar_url);
	KaduParser::registerTag("big_avatar", &get_big_avatar);
	KaduParser::registerTag("big_avatar_url", &get_big_avatar_url);

	avatarActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "refreshAvatarAction",
		this, SLOT(refreshAvatarActionActivated(QAction *, bool)),
		"GG Avatars", tr("Refresh Avatar"), false
	);
	UserBox::insertActionDescription(0, avatarActionDescription);
}

GaduAvatars::~GaduAvatars()
{
	UserBox::removeActionDescription(avatarActionDescription);
	delete avatarActionDescription;
	avatarActionDescription = 0;

	KaduParser::unregisterTag("avatar", &get_avatar);
	KaduParser::unregisterTag("avatar_url", &get_avatar_url);
	KaduParser::unregisterTag("big_avatar", &get_big_avatar);
	KaduParser::unregisterTag("big_avatar_url", &get_big_avatar_url);

	disconnect(linkDownloader, SIGNAL(requestFinished(int, bool)), this, SLOT(gotResponse(int, bool)));
	delete linkDownloader;

	disconnect(fileDownloader, SIGNAL(requestFinished(int, bool)), this, SLOT(fileDownloaded(int, bool)));
	delete fileDownloader;
}

void GaduAvatars::refreshAvatarActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	
	UinsList uins;
	int uin;
	QString path = ggPath() + suffix;
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (window)
	{
		UserListElements users = window->userListElements();
		if (users.count() > 0)
			foreach(const UserListElement &user, users)
			{
				uin = user.ID("Gadu").toUInt();
				QFile file(path + QString::number(uin));
				file.remove();
				file.setFileName(path + QString::number(uin) + "_big");
				file.remove();
				getAvatar(uin, MODE_SMALL);
				getAvatar(uin, MODE_BIG);
			}
	}

	kdebugf2();
}

QString GaduAvatars::getAvatar(int uin, int mode)
{
	QString filename = ggPath() + suffix + QString::number(uin);
	if (mode == MODE_BIG)
		filename += "_big";
	if (QFileInfo(filename).size() > 0)
	{
#ifdef Q_OS_WIN
		/* escape the spaces in filename  - for kadu parser */
		return filename.replace(" ", "\\ ");
#else
		return filename;
#endif
	}
	else
	{
		QBuffer *buffer = new QBuffer();
		int id = linkDownloader->get("/avatars/" + QString::number(uin) + "/0.xml", buffer);
		buffers.insert(id, buffer);
		uins.insert(id, uin);
		modes.insert(id, mode);
		return icons_manager->iconPath("ManageUsersWindowIcon");
	}
}

void GaduAvatars::gotResponse(int id, bool error)
{
	int uin = uins[id];
	int mode = modes[id];

	QBuffer *buffer = buffers[id];
	QString response(buffer->data());
	uins.remove(id);
	buffers.remove(id);
	modes.remove(id);
	delete buffer;
	
	if (!response.isEmpty())
	{
		int begin, end;
		kdebugm(KDEBUG_INFO, "Response:\n%s\n", qPrintable(response));
		if (mode == MODE_SMALL)
		{
			begin = response.indexOf("<originSmallAvatar>") + sizeof("<originSmallAvatar>");
			end = response.indexOf("</originSmallAvatar>");
		}
		else
		{
			begin = response.indexOf("<originBigAvatar>") + sizeof("<originBigAvatar>");
			end = response.indexOf("</originBigAvatar>");
		}
	
		if ((begin > 0) && (end > begin))
			response = response.mid(begin, end - begin);
	}

	/* Do not cache empty avatars */
	if (response.contains("avatar-empty.gif"))
	{
		return;
	}

	QDir dir;
	QString path = ggPath() + suffix;
	dir.mkdir(path, true);
	path += QString::number(uin);
	if (mode == MODE_BIG)
		path += "_big";

	if (QFile::exists(path))
	{
		return;
	}

	QFile *file = new QFile(path);
	if (file->open(QIODevice::WriteOnly))
	{
		QUrl url(response);
		fileDownloader->setHost(url.host());

		QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
		int id = fileDownloader->get(path, file);
		files.insert(id, file);
	}
}

void GaduAvatars::fileDownloaded(int id, bool error)
{
	QFile *file = files[id];
	if (file == NULL)
	{
		return;
	}
	
	file->close();
	if (error)
	{
		file->remove();
	}
	files.remove(id);
	delete file;
}

void GaduAvatars::configurationUpdated()
{
}

void GaduAvatars::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}
