/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QFontMetrics>
#include <QtGui/QLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QPushButton>
#include <QtGui/QToolTip>
#include <QtCore/QProcess>

#include "message_box.h"
#include "icons_manager.h"

#include "showforecastframe.h"
#include "weather_global.h"
#include "textprogress.h"
#include "citysearchresult.h"

/*
	ShowForecastFrameBase
*/

ShowForecastFrameBase::ShowForecastFrameBase(QWidget *parent) :
	QFrame(parent),
	currentPage_(-1)
{
	setAttribute(Qt::WA_DeleteOnClose);

	fieldTranslator_.insert("Pressure", tr("Pressure"));
	fieldTranslator_.insert("Rain", tr("Rain"));
	fieldTranslator_.insert("Snow", tr("Snow"));
	fieldTranslator_.insert("Wind speed", tr("Wind speed"));
	fieldTranslator_.insert("Description", tr("Description"));
	fieldTranslator_.insert("Humidity", tr("Humidity"));
	fieldTranslator_.insert("Dew point", tr("Dew point"));
	fieldTranslator_.insert("Visibility", tr("Visibility"));

	downloadMessage_ = new TextProgress(this);
	downloadMessage_->setMinimumWidth(downloadMessage_->fontMetrics().maxWidth() * 20);
	downloadMessage_->setAlignment(Qt::AlignCenter);
	downloadMessage_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	downloadMessage_->hide();

	errorMessage_ = new QLabel(this);
	errorMessage_->setAlignment(Qt::AlignTop);
	errorMessage_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	errorMessage_->hide();

	forecastFrame_ = new QFrame(this);
	forecastFrame_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	forecastFrame_->hide();

	header_ = new QLabel(forecastFrame_);
	header_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	QFrame *separator = new QFrame(forecastFrame_);
	separator->setFrameShape(HLine);
	separator->setFrameShadow(Sunken);
	separator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	icon_ = new QLabel(forecastFrame_);
	icon_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	icon_->setAlignment(Qt::AlignCenter);
	icon_->setMargin(10);

	temperature_ = new QLabel(forecastFrame_);
	temperature_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	temperature_->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	description_ = new QLabel(forecastFrame_);
	description_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	description_->setTextFormat(Qt::PlainText);
	description_->setAlignment(Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak);
	description_->setMinimumWidth(description_->fontMetrics().maxWidth() * 15);

	QGridLayout *forecastLay = new QGridLayout(forecastFrame_, 4, 2);
	forecastLay->setSpacing(5);
	forecastLay->setMargin(0);
	forecastLay->addMultiCellWidget(header_, 0, 0, 0, 1);
	forecastLay->addMultiCellWidget(separator, 1, 1, 0, 1);
	forecastLay->addMultiCellWidget(icon_, 2, 2, 0, 0);
	forecastLay->addMultiCellWidget(temperature_, 3, 3, 0, 0);
	forecastLay->addMultiCellWidget(description_, 2, 3, 1, 1);

	buttonGroup_ = new QButtonGroup(this);
	buttonGroup_->setExclusive(true);
	buttonBox_ = new QWidget(this);
	buttonBox_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	buttonBox_->hide();
	buttonLayout_ = new QHBoxLayout(buttonBox_);
	buttonLayout_->setSpacing(1);
	//buttonLayout_->setInsideMargin(0);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(5);
	mainLayout->setMargin(5);
	mainLayout->addWidget(downloadMessage_, 0, Qt::AlignJustify);
	mainLayout->addWidget(errorMessage_, 0, Qt::AlignJustify);
	mainLayout->addWidget(forecastFrame_, 0);
	mainLayout->addWidget(buttonBox_, 0, Qt::AlignBottom);

	//QAction* actionCopy = new QAction( tr("Copy"), this );
	//QAction* actionGoToWebPage = new QAction( tr("Go to Web page"), this );
	//QAction* actionChangeCity = new QAction( tr("Change city..."), this );

	contextMenu_ = new QMenu(this);
	contextMenu_->addAction(tr("Copy"), this, SLOT(menuCopy()));
	contextMenu_->addAction(tr("Go to Web page"), this, SLOT(menuGoToPage()));
	contextMenu_->addAction(tr("Change city..."), this, SIGNAL(changeCity()));

	connect(buttonGroup_, SIGNAL(buttonClicked (int)), this, SLOT(dayClicked(int)));
	connect(&downloader_, SIGNAL(finished()), this, SLOT(downloadingFinished()));
	connect(&downloader_, SIGNAL(error(GetForecast::ErrorId,QString)), this, SLOT(downloadingError(GetForecast::ErrorId, QString)));
}

void ShowForecastFrameBase::start(const CitySearchResult &city)
{
	downloadMessage_->setText(tr("Forecast download"));
	downloadMessage_->show();
	downloader_.downloadForecast(city.server_, city.cityId_);
}

void ShowForecastFrameBase::downloadingFinished()
{
	downloadMessage_->hide();
	showForecast();
}

void ShowForecastFrameBase::downloadingError(GetForecast::ErrorId err, QString url)
{
	downloadMessage_->hide();

	errorMessage_->setAlignment(Qt::AlignCenter);
	if (err == GetForecast::Connection)
		errorMessage_->setText(tr("Cannot load page %1").arg(url));
	else if (err == GetForecast::Parser)
		errorMessage_->setText(tr("Parse error page %1").arg(url));

	errorMessage_->show();
}

void ShowForecastFrameBase::showForecast()
{
	int i = 0;
	foreach (const ForecastDay &day, downloader_.getForecast().Days)
	{
		QPushButton *btn = new QPushButton(buttonBox_);
		btn->setToggleButton(true);
		btn->setPixmap(QPixmap(day["Icon"]));
		btn->show();
		buttonLayout_->addWidget(btn);
		buttonGroup_->addButton(btn, i++);

		QToolTip::add(btn, "<b>" + day["Name"] + "</b><br>" + day["Temperature"]);
	}

	buttonGroup_->button(0)->setChecked(true);
	dayClicked(0);
	forecastFrame_->show();
	buttonBox_->show();
}

void ShowForecastFrameBase::dayClicked(int id)
{
	setCurrentPage(id);
	emit dayChanged(currentPage_);
}

void ShowForecastFrameBase::mousePressEvent(QMouseEvent *e)
{
	if (e->button() != Qt::RightButton || currentPage_ < 0)
		return;

	contextMenu_->popup(mapToGlobal(e->pos()));
}

void ShowForecastFrameBase::menuCopy()
{
	const ForecastDay &day = downloader_.getForecast().Days[currentPage_];
	QString Data = downloader_.getForecast().LocationName  + " - " + day["Name"] + '\n';

	bool first = true;
	for (ForecastDay::const_iterator it = day.begin(); it != day.end(); it++)
	{
		if (it.key() != "Name" && it.key() != "Icon")
		{
			if (first)
				first = false;
			else
				Data += '\n';
			Data += getFieldTranslation(it.key() ) + ": " + it.data();
		}
	}

	Data.replace("&deg;","�");
	Data.replace("&nbsp;"," ");
	QApplication::clipboard()->setText(Data);
}

void ShowForecastFrameBase::menuGoToPage()
{
	QStringList args = QStringList::split(" ", config_file.readEntry("Chat","WebBrowser"));

	PlainConfigFile wConfig( WeatherGlobal::getConfigPath(downloader_.getForecast().config));
	QString url = wConfig.readEntry("Default","Default host") + '/' + wConfig.readEntry("Default", "Default path");
	url.replace("%s", downloader_.getForecast().LocationID);
	args.push_back(url);

	QString program = args.front();
	args.pop_front();
	QProcess::execute(program, args);
}

const QString & ShowForecastFrameBase::getFieldTranslation(const QString &field)
{
	QMap<QString, QString>::const_iterator translation = fieldTranslator_.find(field);
	if (translation != fieldTranslator_.end())
		return translation.data();
	else
		return field;
}

void ShowForecastFrameBase::setCurrentPage(int page)
{
	currentPage_ = page;
	const ForecastDay& day = downloader_.getForecast().Days[currentPage_];
	header_->setText("<b><u>" + downloader_.getForecast().LocationName + "</u> " + day["Name"]);
	icon_->setPixmap(QPixmap(day["Icon"]));
	temperature_->setText( "<b>"+day["Temperature"]+"</b>" );
	bool first = true;
	QString description;

	for (ForecastDay::const_iterator it = day.begin(); it != day.end(); it++)
	{
		if (it.key() != "Name" && it.key() != "Icon" && it.key() != "Temperature")
		{
			if (first)
				first = false;
			else
				description += '\n';


			description += getFieldTranslation(it.key()) + ": " + it.data();
		}
	}
	description_->setText(description);
}

/*
	ShowForecastFrame
*/

ShowForecastFrame::ShowForecastFrame(QWidget *parent, const CitySearchResult &city)
:
	ShowForecastFrameBase(parent),
	city_(city),
	started_(false)
{

}

void ShowForecastFrame::start()
{
	if (!started_)
	{
		started_ = true;
		ShowForecastFrameBase::start(city_);
	}
}

/*
	SearchAndShowForecastFrame
*/

SearchAndShowForecastFrame::SearchAndShowForecastFrame(QWidget *parent, QString city, QString serverConfigFile) :
	ShowForecastFrameBase(parent),
	city_(city),
	server_(serverConfigFile),
	started_(false)
{
	connect(&search_, SIGNAL(finished()), this, SLOT(finished()));
	connect(&search_, SIGNAL(error(QString)), this, SLOT(error(QString)));
}

void SearchAndShowForecastFrame::start()
{
	if (!started_)
	{
		started_ = true;
		downloadMessage_->setText(tr("Searching for <b>%1</b>").arg(city_));
		downloadMessage_->show();
		search_.findID(city_, server_);
	}
}

void SearchAndShowForecastFrame::finished()
{
	const CITYSEARCHRESULTS& results = search_.getResult();

	if (results.empty())
	{
		downloadMessage_->hide();
		errorMessage_->show();
		errorMessage_->setAlignment(Qt::AlignCenter);
		errorMessage_->setText(tr("<b>%1</b> not found.").arg(city_));
	}
	else
		ShowForecastFrameBase::start(results.at(0));
}

void SearchAndShowForecastFrame::error(QString url)
{
	downloadMessage_->hide();
	errorMessage_->show();
	errorMessage_->setAlignment(Qt::AlignCenter);
	errorMessage_->setText(tr("Cannot load page %1.").arg(url));
}
