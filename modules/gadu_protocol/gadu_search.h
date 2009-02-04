/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_SEARCH_H
#define GADU_SEARCH_H

#include "protocols/status.h"

struct SearchResult
{
	QString Uin;
	QString First;
	QString Last;
	QString Nick;
	QString Born;
	QString City;
	QString FamilyName;
	QString FamilyCity;
	int Gender;
	Status Stat;

	SearchResult();
	SearchResult(const SearchResult &);
	void setData(const char *uin, const char *first, const char *last, const char *nick, const char *born,
		const char *city, const char *familyName, const char *familyCity, const char *gender, const char *status);
};

typedef QList<SearchResult> SearchResults;

struct SearchRecord
{
	int Seq;
	int FromUin;
	QString Uin;
	QString FirstName;
	QString LastName;
	QString NickName;
	QString City;
	QString BirthYearFrom;
	QString BirthYearTo;
	int Gender;
	bool Active;
	bool IgnoreResults;

	SearchRecord();
	virtual ~SearchRecord();

	void reqUin(const QString& uin);
	void reqFirstName(const QString& firstName);
	void reqLastName(const QString& lastName);
	void reqNickName(const QString& nickName);
	void reqCity(const QString& city);
	void reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo);
	void reqGender(bool female);
	void reqActive();

	void clearData();
};

#endif // GADU_SEARCH_H
