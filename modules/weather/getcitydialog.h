/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GETCITYDIALOG_H
#define GETCITYDIALOG_H

#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QListWidget>
#include <QtGui/QStringListModel>

#include "gadu.h"
#include "userlistelement.h"

#include "weather_global.h"
#include "search_location_id.h"

class TextProgress;
class QComboBox;

class EnterCityDialog : public QDialog
{
	Q_OBJECT
	
	public:
		EnterCityDialog( UserListElement user = UserListElement(), const QString& cityName = QString::null );
		
	private:
		UserListElement user_;
		QComboBox* cityEdit_;
		
	private slots:
		void findClicked();
};

class SearchingCityDialog : public QDialog
{
	Q_OBJECT
	
	public:
		SearchingCityDialog( UserListElement user = UserListElement(), const QString& cityName = QString::null );
		void show();
		
	private:
		void findCity( const QString& name );
		
	private:
		UserListElement user_;
		QString cityName_;
		SearchLocationID searchId_;
		TextProgress* progress_;
		
	private slots:
		void userCitySearch(SearchResults& searchResults, int seq, int fromUin);
		void nextServerSearch( const QString& city, const QString& serverName_ );
		void searchFinished();
		void cancelClicked();
};

class SelectCityDialog : public QDialog
{
	Q_OBJECT
			
	public:
		SelectCityDialog( UserListElement user, const QString& cityName, const CITYSEARCHRESULTS& results );
		
	private:
		UserListElement user_;
		QString cityName_;
		CITYSEARCHRESULTS results_;
        QListWidget* cityList_;
        QStringListModel* cityModel_;
		
	private slots:
		void okClicked();
		void newSearchClicked();
		void showCity( QListWidgetItem* item );
		
};

#endif // GETCITYDIALOG_H
