/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>

#include "config_file.h"
#include "kadu_parser.h"
#include "misc.h"

#include "preview.h"

Preview::Preview(QWidget *parent, char *name)
	: KaduTextBrowser(parent, name)
{
	setMinimumHeight(170);

	UserStatus status;
	status.setBusy(qApp->translate("@default", "Description"));

	ule.addProtocol("Gadu", "999999");
	ule.setStatus("Gadu", status);
	ule.setFirstName(qApp->translate("@default", "Mark"));
	ule.setLastName(qApp->translate("@default", "Smith"));
	ule.setNickName(qApp->translate("@default", "Jimbo"));
	ule.setAltNick(qApp->translate("@default", "Jimbo"));
	ule.setMobile("+48123456789");
	ule.setEmail("jimbo@mail.server.net");
	ule.setHomePhone("+481234567890");
	ule.setAddressAndPort("Gadu", QHostAddress(2130706433), 80);
	ule.setDNSName("Gadu", "host.server.net");
}

Preview::~Preview()
{
}

void Preview::syntaxChanged(const QString &content)
{
	viewport()->setUpdatesEnabled(false);

	QString syntax = content;
	emit needSyntaxFixup(syntax);

	QString text;

	setText("<body bgcolor=\"" + resetBackgroundColor + "\"></body>");
	int count = objectsToParse.count();

	if (count)
		for (int i = 0; i < count; i++)
			text += KaduParser::parse(syntax, ules[i], objectsToParse.at(i));
	else
		text = KaduParser::parse(syntax, ule);

	setText(text);
	emit needFixup(this);

	viewport()->setUpdatesEnabled(true);
	viewport()->repaint();
}
