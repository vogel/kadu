/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QBuffer>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include "gg_avatars.h"

#include "debug.h"
#include "modules.h"
#include "kadu_parser.h"
#include "config_file.h"
#include "userlistelement.h"

QDateTime started;
GaduAvatars *gaduAvatars;

extern "C" KADU_EXPORT int gg_avatars_init()
{
	kdebugf();

	gaduAvatars = new GaduAvatars();
//	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/gg_avatars.ui"), GaduAvatars);

	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void gg_avatars_close()
{
	kdebugf();

//	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/gg_avatars.ui"), GaduAvatars);
	delete gaduAvatars;
	gaduAvatars = NULL;

	kdebugf2();
}

QString get_avatar(const UserListElement &ule)
{
	return gaduAvatars->getAvatar(ule);
}

GaduAvatars::GaduAvatars()
{
	KaduParser::registerTag("avatar", &get_avatar);
}

GaduAvatars::~GaduAvatars()
{
	KaduParser::unregisterTag("avatar", &get_avatar);
}

QString GaduAvatars::getAvatar(const UserListElement &ule)
{
	int uin = ule.ID("Gadu").toInt();
	if (avatars.contains(uin))
	{
		return avatars[uin];
	}
	else
	{
		SyncHTTP h("api.gadu-gadu.pl");
		QBuffer buff;
    		h.syncGet("/avatars/" + ule.ID("Gadu") + "/0.xml", &buff);

		QString response(buff.data());

		if (!response.isEmpty())
		{
			int begin = response.indexOf("<smallAvatar>") + 13;
			int end = response.indexOf("</smallAvatar>");
			
			if ((begin > 0) && (end > begin))
				response = response.mid(begin, end - begin);
			response = "<img src=\"" + response + "\"/>";
		}
		avatars.insert(uin, response);

		return response;
	}
}

void GaduAvatars::configurationUpdated()
{
}

void GaduAvatars::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}
