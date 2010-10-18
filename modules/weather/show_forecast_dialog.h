/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SHOW_FORECAST_DIALOG_H
#define SHOW_FORECAST_DIALOG_H

#include <QtGui/QDialog>

#include "userlistelement.h"

class CitySearchResult;

/**
	\class ShowForecastDialog
	Wy�wietla w okienku prognoz� pogody
	pobran� za pomoc� klasy GetForecast
**/
class ShowForecastDialog : public QDialog
{
	Q_OBJECT

	UserListElement user_;

private slots:
	void tabChanged(QWidget *page);
	virtual void changeCity();

public:
	ShowForecastDialog(const CitySearchResult &city, UserListElement user = UserListElement());
	virtual ~ShowForecastDialog() {}
};

#endif // SHOW_FORECAST_DIALOG_H
