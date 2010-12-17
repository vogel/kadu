/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GET_CITY_DIALOG_H
#define GET_CITY_DIALOG_H

#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QListWidget>
#include <QtGui/QStringListModel>

#include "gadu.h"
#include "userlistelement.h"

#include "weather_global.h"
#include "search_location_id.h"

class QComboBox;

class TextProgress;

class EnterCityDialog : public QDialog
{
	Q_OBJECT
private:
	UserListElement user_;
	QComboBox *cityEdit_;

private slots:
	void findClicked();

public:
	EnterCityDialog(UserListElement user = UserListElement(), const QString &cityName = QString());
};

class SearchingCityDialog : public QDialog
{
	Q_OBJECT
	
	void findCity(const QString &name);

private:
	UserListElement user_;
	QString cityName_;
	SearchLocationID searchId_;
	TextProgress *progress_;

private slots:
	void userCitySearch(SearchResults &searchResults, int seq, int fromUin);
	void nextServerSearch(const QString &city, const QString &serverName_);
	void searchFinished();
	void cancelClicked();

public:
	SearchingCityDialog(UserListElement user = UserListElement(), const QString &cityName = QString());
	void show();
};

class SelectCityDialog : public QDialog
{
	Q_OBJECT

	UserListElement user_;
	QString cityName_;
	CITYSEARCHRESULTS results_;
	QListWidget *cityList_;
	QStringListModel *cityModel_;

private slots:
	void okClicked();
	void newSearchClicked();
	void showCity(QListWidgetItem *item);

public:
	SelectCityDialog(UserListElement user, const QString &cityName, const CITYSEARCHRESULTS &results);
};

#endif // GET_CITY_DIALOG_H
