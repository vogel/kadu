/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QVector>

#include "misc/misc.h"

#include "weather_parser.h"
#include "weather_global.h"

WeatherParser::WeatherParser()
{
}

/**
	Formatowanie wyjsciowej prognozy pogody
**/
bool WeatherParser::getData(const QString &page, const PlainConfigFile *wConfig, Forecast &forecast) const
{
	kdebugf();

	bool cs = wConfig->readBoolEntry("Default","CaseSensitive");

	int num_layouts = wConfig->readNumEntry("Default","Layouts");
	WDataValue dValue;

	QString actualName;
	QVector<WDataValue> valsList;
	QStringList nextDaysNamesList;
	QVector<WDataValue> nextDaysValsList;

	bool result;
	int cursor = 0;
	for (int i = 0; i < num_layouts; i++)
	{
		QString layoutSection = QString("Layout%1").arg(i + 1);
		QString layoutName = wConfig->readEntry(layoutSection,"Name");
		int num_repeats = wConfig->readNumEntry(layoutSection,"Repeats");
		int num_values = wConfig->readNumEntry(layoutSection,"Values");

		for (int j = 0; j < num_repeats; j++)
		{
			for (int k = 0; k<num_values; k++)
			{
				QString valSection = QString("Layout%1Value%2").arg(i + 1).arg(k + 1);

				dValue.Name = wConfig->readEntry(valSection,"Name");
				dValue.Start = wConfig->readEntry(valSection,"Start");
				dValue.End = wConfig->readEntry(valSection,"End");
				dValue.Content = "";

				result = getDataValue(page, dValue,cursor, wConfig, cs);

				if (dValue.Content.isEmpty())
					continue;
				else if (dValue.Name == "Phantom")
					continue;

				if (layoutName == "Name")
					forecast.LocationName = dValue.Content;
				else if (layoutName == "Actual")
				{
					if (dValue.Name == "Name")
						actualName = dValue.Content;
					else
						valsList.push_back(dValue);
				}
				else if (layoutName == "DaysNames")
				{
					if (dValue.Name == "Actual")
						actualName = dValue.Content;
					else if (dValue.Name == "NextDays")
						nextDaysNamesList.push_back(dValue.Content);
				}
				else if (layoutName == "NextDays" && dValue.Name == "Name")
					nextDaysNamesList.push_back(dValue.Content);
				else
					nextDaysValsList.push_back(dValue);
			}
		}
	}

	QString Data("");

	ForecastDay fDay;

	fDay["Name"] = actualName;
	for (QVector<WDataValue>::iterator it = valsList.begin(); it != valsList.end(); it++)
	{
		if ((*it).Name == "Icon")
			fDay["Icon"] = WeatherGlobal::getIconPath(wConfig->readEntry("Icons", (*it).Content));
		else
			fDay[(*it).Name] = (*it).Content;
	}

	forecast.Days.push_back(fDay);
	fDay.clear();

	int num_vals = nextDaysValsList.count();
	int num_names = nextDaysNamesList.count();

	if (!num_names)
		return false;

	int T = num_vals / num_names;

	if (!T)
		return false;

	Data = "";
	num_vals = T * num_names; /* Dorr: fix for index out of bounds when parsing data from
					Onet (num vals 31, num names 5, crash at i = 30 ->
					nextDaysNamesList[5] which is having 5 elements) */
	for (int i = 0; i < num_vals; i++)
	{
		WDataValue& val = nextDaysValsList[i];

		if (i % T == 0)
			fDay["Name"] = nextDaysNamesList[i / T];

		if (val.Name == "Icon")
			fDay["Icon"] = WeatherGlobal::getIconPath(wConfig->readEntry("Icons",val.Content));
		else
			fDay[val.Name] = val.Content;

		if (i % T == T - 1)
		{
			forecast.Days.push_back(fDay);
			fDay.clear();

			Data = "";
		}
	}

	kdebugf2();
	return true;
}

/**
	Wylawianie fragmentu ze strony
**/
bool WeatherParser::getDataValue(const QString &page, WDataValue &wdata, int &cursor, const PlainConfigFile *wConfig, bool CaseSensitive) const
{
	kdebugf();

	long int start, end;
	long int startData;

	start = page.find(wdata.Start, cursor, CaseSensitive);
	if (start == -1)
		return false;

	startData = start + wdata.Start.length();

	end = page.find(wdata.End, startData, CaseSensitive);
	if (end == -1)
		return false;

	cursor = end;
	wdata.Content = tagClean(page.mid(startData, end-startData));

	kdebugf2();
	return true;
}

/**
	Zwraca wyszukane identyfikatory miast dla szukanego miasta
**/
void WeatherParser::getSearch(const QString &page, const PlainConfigFile *wConfig, const QString &serverConfigFile, CITYSEARCHRESULTS *results) const
{
	kdebugf();

	bool CaseSensitive = wConfig->readBoolEntry("Default","CaseSensitive");

	long int start, separator, end, current;
	long int startData, sepData;
	bool idFirst;
	QString section, starttag, septag, endtag, first, second;

	int countResults = wConfig->readNumEntry("Name Search","SearchResults");

	for (int i = 0; i < countResults; i++)
	{
		section = QString("SearchResult%1").arg(i + 1);
		idFirst = wConfig->readBoolEntry(section,"IDFirst");
		starttag = wConfig->readEntry(section,"Start");
		septag = wConfig->readEntry(section,"Separator");
		endtag = wConfig->readEntry(section,"End");

		current = 0;
		do
		{
			start = page.find(starttag, current, CaseSensitive);
			startData = start + starttag.length();
			separator = page.find(septag, startData, CaseSensitive);
			sepData = separator + septag.length();
			end = page.find(endtag, sepData, CaseSensitive);

			if (end != -1 && start != -1 && separator != -1)
			{
				first = page.mid(startData, separator-startData);
				second = page.mid(sepData, end-sepData);

				if (!first.isEmpty() && !second.isEmpty())
				{
					if (idFirst)
						results->push_back(CitySearchResult(tagClean(second), first, serverConfigFile));
					else
						results->push_back(CitySearchResult(tagClean(first), second, serverConfigFile));
				}
				current = end + endtag.length();
			}
		}
		while (end != -1 && start != -1 && separator != -1);
	}

	kdebugf2();
}

/**
	Wylowienie szukanego ID miasta z adresu URL.
	(niekt�re serwisy przekierowywuja od razu na
	strone z prognoza, gdy znajda tylko jedno miasto)
**/
QString WeatherParser::getFastSearch(const QString &link, const PlainConfigFile *wConfig) const
{
	kdebugf();

	QString starttag, endtag;
	long int start, end, startData;

	starttag = wConfig->readEntry("Name Search","FastSearch Start");
	endtag = wConfig->readEntry("Name Search","FastSearch End");

	start = link.find(starttag, 0, false);
	startData = start + starttag.length();

	if (endtag.isEmpty())
		end = link.length();
	else
		end = link.find(endtag, startData, false);

	kdebugf2();

	if (start == -1 || end == -1)
		return "";
	else
		return link.mid(startData, end-startData);
}

/**
	Czyszczenie bufora z tag�w HTML i zamienianie znakow specjalnych
**/
QString WeatherParser::tagClean(QString str) const
{
	kdebugf();

	//str.replace("&deg;","�");
	str.replace("&nbsp;"," ");

	int start, end;
	start = 0;
	do
	{
		start = str.find("<",start);
		end = str.find(">",start+1);

		if (start != -1 && end != -1)
			str.replace(start,end+1-start, " ");

	}
	while (start != -1 && end != -1);

	str = replacedNewLine(str, QLatin1String(" "));
	str.replace("  ", " ");
	str.replace(" ,", ",");
	str.replace(" .", ".");
	str.replace(" :", ":");
	str.replace(" / ", "/");

	kdebugf2();
	return str;
}

