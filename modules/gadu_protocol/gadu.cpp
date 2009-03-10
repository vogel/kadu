/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QIntValidator>

#ifdef Q_WS_WIN
#include <winsock2.h>
#undef MessageBox
#else
#include <netinet/in.h>
#endif

#include <time.h>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "contacts/contact-manager.h"
#include "contacts/ignored-helper.h"

#include "protocols/protocols_manager.h"
#include "protocols/status.h"

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"

#include "helpers/gadu-formatter.h"
#include "helpers/gadu-importer.h"

#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"
#include "socket-notifiers/gadu-token-socket-notifiers.h"

#include "gadu-account.h"
#include "gadu-contact-account-data.h"
#include "gadu_images_manager.h"
#include "gadu-protocol.h"
#include "gadu_protocol_factory.h"
#include "gadu-search-record.h"
#include "gadu-search-result.h"
#include "uins_list.h"

#include "xml_config_file.h"

#include "gadu.h"

extern "C" int gadu_protocol_init(bool firstLoad)
{
	if (ProtocolsManager::instance()->hasProtocolFactory("gadu"))
		return 0;

	ProtocolsManager::instance()->registerProtocolFactory("gadu", new GaduProtocolFactory());

	if (!xml_config_file->hasNode("Accounts"))
		GaduImporter::instance()->importAccounts();

	GaduImporter::instance()->importContacts();

	return 0;
}

extern "C" void gadu_protocol_close()
{
	ProtocolsManager::instance()->unregisterProtocolFactory("gadu");
}

// ------------------------------------
//              Timers
// ------------------------------------

// ------------------------------------
//        Timers - implementation
// ------------------------------------
/*
SearchResult::SearchResult() :
	Uin(), First(), Last(), Nick(), Born(), City(),
	FamilyName(), FamilyCity(), Gender(0), Stat()
{
}

SearchResult::SearchResult(const SearchResult &copyFrom) :
	Uin(copyFrom.Uin),
	First(copyFrom.First),
	Last(copyFrom.Last),
	Nick(copyFrom.Nick),
	Born(copyFrom.Born),
	City(copyFrom.City),
	FamilyName(copyFrom.FamilyName),
	FamilyCity(copyFrom.FamilyCity),
	Gender(copyFrom.Gender),
	Stat(copyFrom.Stat)
{
}

void SearchResult::setData(const char *uin, const char *first, const char *last, const char *nick, const char *born,
	const char *city, const char *familyName, const char *familyCity, const char *gender, const char *status)
{
	kdebugf();
	Uin = cp2unicode(uin);
	First = cp2unicode(first);
	Last = cp2unicode(last);
	Nick = cp2unicode(nick);
	Born = cp2unicode(born);
	City = cp2unicode(city);
	FamilyName = cp2unicode(familyName);
	FamilyCity = cp2unicode(familyCity);
	if (status)
		Stat = GaduProtocol::typeToStatus(atoi(status) & 0x7F);
	if (gender)
		Gender = atoi(gender);
	else
		Gender = 0;
	kdebugf2();
}

SearchRecord::SearchRecord() :
	Seq(0), FromUin(0), Uin(), FirstName(), LastName(), NickName(), City(),
	BirthYearFrom(), BirthYearTo(), Gender(0), Active(false), IgnoreResults(false)
{
	kdebugf();
	kdebugf2();
}

SearchRecord::~SearchRecord()
{
}

void SearchRecord::reqUin(const QString &uin)
{
	Uin = uin;
}

void SearchRecord::reqFirstName(const QString &firstName)
{
	FirstName = firstName;
}

void SearchRecord::reqLastName(const QString &lastName)
{
	LastName = lastName;
}

void SearchRecord::reqNickName(const QString &nickName)
{
	NickName = nickName;
}

void SearchRecord::reqCity(const QString &city)
{
	City = city;
}

void SearchRecord::reqBirthYear(const QString &birthYearFrom, const QString &birthYearTo)
{
	BirthYearFrom = birthYearFrom;
	BirthYearTo = birthYearTo;
}

void SearchRecord::reqGender(bool female)
{
	Gender = (female ? 2 : 1);
}

void SearchRecord::reqActive()
{
	Active = true;
}

void SearchRecord::clearData()
{
	kdebugf();
	FromUin = 0;
	Uin.truncate(0);
	FirstName.truncate(0);
	LastName.truncate(0);
	NickName.truncate(0);
	City.truncate(0);
	BirthYearFrom.truncate(0);
	BirthYearTo.truncate(0);
	Gender = 0;
	Active = false;
	IgnoreResults = false;
	kdebugf2();
}*/
