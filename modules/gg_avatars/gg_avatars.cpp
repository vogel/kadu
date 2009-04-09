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
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QBuffer>
#include <QtCore/QFileInfo>
#include <QtNetwork/QHttp>

#include "gg_avatars.h"

#include "debug.h"
#include "modules.h"
#include "kadu_parser.h"
#include "config_file.h"
#include "userlistelement.h"

GaduAvatars *gaduAvatars;

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

QString get_avatar(const UserListElement &ule)
{
	return "<img src=\"" + gaduAvatars->getAvatar(ule)+ "\"/>";
}

QString get_avatar_url(const UserListElement &ule)
{
	return gaduAvatars->getAvatar(ule);
}


GaduAvatars::GaduAvatars()
{
	linkDownloader = new QHttp("api.gadu-gadu.pl");
	connect(linkDownloader, SIGNAL(requestFinished(int, bool)), this, SLOT(gotResponse(int, bool)));

	fileDownloader = new QHttp();
	connect(fileDownloader, SIGNAL(requestFinished(int, bool)), this, SLOT(fileDownloaded(int, bool)));

	KaduParser::registerTag("avatar", &get_avatar);
	KaduParser::registerTag("avatar_url", &get_avatar_url);
}

GaduAvatars::~GaduAvatars()
{
	KaduParser::unregisterTag("avatar", &get_avatar);
	KaduParser::registerTag("avatar_url", &get_avatar_url);

	disconnect(linkDownloader, SIGNAL(requestFinished(int, bool)), this, SLOT(gotResponse(int, bool)));
	delete linkDownloader;

	disconnect(fileDownloader, SIGNAL(requestFinished(int, bool)), this, SLOT(fileDownloaded(int, bool)));
	delete fileDownloader;
}

QString GaduAvatars::getAvatar(const UserListElement &ule)
{
	int uin = ule.ID("Gadu").toInt();
	QString filename = ggPath() + "/avatars/" + QString::number(uin);
	if (QFileInfo(filename).size() > 0)
	{
		return "file://" + filename;
	}
	else
	{
		QBuffer *buffer = new QBuffer();
    		int id = linkDownloader->get("/avatars/" + ule.ID("Gadu") + "/0.xml", buffer);
		buffers.insert(id, buffer);
		uins.insert(id, uin);
		return "";
	}
}

void GaduAvatars::gotResponse(int id, bool error)
{
	int uin = uins[id];

	QBuffer *buffer = buffers[id];
	QString response(buffer->data());
	uins.remove(id);
	buffers.remove(id);
	delete buffer;
	
	if (!response.isEmpty())
	{
		int begin = response.indexOf("<smallAvatar>") + 13;
		int end = response.indexOf("</smallAvatar>");
	
		if ((begin > 0) && (end > begin))
			response = response.mid(begin, end - begin);
	}

	QDir dir;
	QString path = ggPath() + "/avatars/";
	dir.mkdir(path, true);
	path += QString::number(uin);

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
	if (error) {
		printf("Error\n");
		fflush(stdout);
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
