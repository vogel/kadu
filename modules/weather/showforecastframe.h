/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SHOW_FORECAST_FRAME_H
#define SHOW_FORECAST_FRAME_H

#include <QtCore/QMap>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>

#include "userlistelement.h"

#include "weather_parser.h"
#include "get_forecast.h"
#include "weather_global.h"
#include "search_location_id.h"

class CitySearchResult;

/**
	\class ShowForecastFrameBase
	Wy�wietla w okienku prognoz� pogody
	pobran� za pomoc� klasy GetForecast
**/
class ShowForecastFrameBase : public QFrame
{
	Q_OBJECT
	
	Forecast forecast_;
	GetForecast downloader_;

	QButtonGroup *buttonGroup_;
	QWidget *buttonBox_;
	QHBoxLayout *buttonLayout_;
	QMenu *contextMenu_;
	QMap<QString, QString> fieldTranslator_;

	int currentPage_;

	void showForecast();
	void mousePressEvent(QMouseEvent *e);
	const QString & getFieldTranslation(const QString &field);
	void setCurrentPage(int page);

private slots:
	void downloadingFinished();
	void downloadingError(GetForecast::ErrorId err, QString url);
	void dayClicked(int id);
	void menuCopy();
	void menuGoToPage();

protected:
	QLabel *downloadMessage_;
	QLabel *errorMessage_;
	QFrame *forecastFrame_;
	QLabel *header_;
	QLabel *icon_;
	QLabel *temperature_;
	QLabel *description_;
	void start(const CitySearchResult &city);

public:
	ShowForecastFrameBase(QWidget *parent);
	virtual ~ShowForecastFrameBase() {}
	virtual void start() = 0;

signals:
	void changeCity();
	void dayChanged(int page);
};

/**
	\class ShowForecastFrame
	Wy�wietla w okienku prognoz� pogody
**/
class ShowForecastFrame : public ShowForecastFrameBase
{
	CitySearchResult city_;
	bool started_;

public:
	ShowForecastFrame(QWidget *parent, const CitySearchResult &city);
	virtual ~ShowForecastFrame() {}
	void start();
};

/**
	\class SearchAndShowForecastFrame
	Wyszukuje miasto w serwisie i wy�wietla prognoz� pogody
**/
class SearchAndShowForecastFrame : public ShowForecastFrameBase
{
	Q_OBJECT
	
	QString city_;
	QString server_;
	SearchLocationID search_;
	bool started_;
	
private slots:
	void finished();
	void error(QString url);

public:
	SearchAndShowForecastFrame(QWidget *parent, QString city, QString serverConfigFile);
	virtual ~SearchAndShowForecastFrame() {}
	void start();
};

#endif // SHOW_FORECAST_FRAME_H
