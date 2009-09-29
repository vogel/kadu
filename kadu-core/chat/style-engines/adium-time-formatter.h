/*****************************************************************************
	Adium time formatter
	This class is based on ChatWindowStyle and WeekDate classes from qutIM
	instant	messenger (see: http://www.qutim.org/)

	Copyright (c) 2008-2009 by Rustam Chakin
		      2008-2009 by Nigmatullin Ruslan

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef ADIUM_TIME_FORMATTER_H
#define ADIUM_TIME_FORMATTER_H

#include <QtCore/QDate>
#include <QtCore/QObject>
#include <QtCore/QString>

class WeekDate
{
	int m_year;
	int m_week;
	int m_day;

public:
	inline WeekDate(const QDate &date) { setDate(date); }
	inline WeekDate(int y, int m, int d) { setDate(QDate(y, m, d)); }
	inline int year() const { return m_year; }
	inline int week() const { return m_week; }
	inline int day() const { return m_day; }
	void setDate(const QDate &date);

};

class AdiumTimeFormatter : public QObject
{
	Q_OBJECT

	inline void appendStr(QString &str, const QString &res, int length);
	inline void appendInt(QString &str, int number, int length);
	void finishStr(QString &str, const WeekDate &week_date, const QDate &date, const QTime &time, QChar c, int length);

public:
	AdiumTimeFormatter(QObject *parent = 0);

	/**
	 * It should be equal to NSDateFormatter of MacOS X
	 */
	QString convertTimeDate(const QString &mac_format, const QDateTime &datetime);
};

#endif // ADIUM_TIME_FORMATTER_H
