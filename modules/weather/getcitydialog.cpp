/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QFontMetrics>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"

#include "getcitydialog.h"
#include "textprogress.h"
#include "weather_global.h"
#include "show_forecast_dialog.h"

/*
	EnterCityDialog
*/

EnterCityDialog::EnterCityDialog(UserListElement user, const QString &cityName)
:
	QDialog(0, "EnterCityDialog"),
	user_(user)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setCaption(tr("City search"));

	QWidget *ec = new QWidget(this);
	QHBoxLayout *ecLayout = new QHBoxLayout(ec);
	ecLayout->setSpacing( 5 );
	ecLayout->addWidget(new QLabel(tr("City:"), ec));
	cityEdit_ = new QComboBox(ec);
	cityEdit_->setEditable(true);
	cityEdit_->insertStringList(weather_global->recentLocations_);
	cityEdit_->setCurrentText(cityName);
	cityEdit_->lineEdit()->selectAll();
	cityEdit_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	cityEdit_->setMinimumWidth(cityEdit_->fontMetrics().maxWidth() * 15);
	ecLayout->addWidget(cityEdit_);
	QPushButton *findButton = new QPushButton(icons_manager->loadIcon("LookupUserInfo"),  tr("Find"), this);
	findButton->setDefault(true);

	QVBoxLayout *enterLayout = new QVBoxLayout(this);
	enterLayout->setSpacing(5);
	enterLayout->setMargin(5);
	enterLayout->addWidget(ec, 0);
	enterLayout->addWidget(findButton, 0, Qt::AlignCenter);

	connect(findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
}

void EnterCityDialog::findClicked()
{
	QString cityName = cityEdit_->currentText();
	if (cityName.isEmpty())
		MessageBox::msg(tr("Enter city name!"), false, "Warning");
	else
	{
		weather_global->insertRecentLocation(cityName);
		close();
		SearchingCityDialog *scd = new SearchingCityDialog(user_, cityName);
		scd->show();
		scd->setFocus();
	}
}

/*
	SearchingCityDialog
*/

SearchingCityDialog::SearchingCityDialog(UserListElement user, const QString &cityName) :
	QDialog(0, "SearchingCityDialog"),
	user_(user),
	cityName_(cityName)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setCaption(tr("City search"));

	progress_ = new TextProgress(this);
	progress_->setTextFormat(Qt::PlainText);
	progress_->setAlignment(Qt::AlignCenter | Qt::SingleLine);
	progress_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	progress_->setMinimumWidth(progress_->fontMetrics().maxWidth() * 20);

	QPushButton *cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	cancelButton->setDefault(true);

	QVBoxLayout *searchLayout = new QVBoxLayout(this);
	searchLayout->setSpacing(5);
	searchLayout->setMargin(5);
	searchLayout->addWidget(progress_, 0);
	searchLayout->addWidget(cancelButton, 0, Qt::AlignCenter);

	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	connect(&searchId_, SIGNAL(nextServerSearch( const QString &, const QString &)),
			this, SLOT(nextServerSearch( const QString &, const QString &)));
	connect(&searchId_, SIGNAL(finished()), this, SLOT(searchFinished()));
}

void SearchingCityDialog::show()
{
	if (!cityName_.isEmpty())
	{
		findCity(cityName_);
 		QDialog::show();
	}
	else if (!user_.ID("Gadu").isEmpty())
	{
		// Activating progressLayout_
		progress_->setText( tr("Retrieving city from public directory") );

		connect(gadu, SIGNAL(newSearchResults(SearchResults &, int, int)), this,
				SLOT(userCitySearch(SearchResults &, int, int)));

		SearchRecord searchRecord;
		searchRecord.reqUin(user_.ID("Gadu"));
		gadu->searchInPubdir(searchRecord);
 		QDialog::show();
	}
	else
	{
		close();
		EnterCityDialog* ecd = new EnterCityDialog(user_, cityName_);
		ecd->show();
		ecd->setFocus();
	}
}

void SearchingCityDialog::findCity(const QString &name)
{
	cityName_ = name;
	progress_->clearProgressText();
	searchId_.findID(name);
}

void SearchingCityDialog::userCitySearch(SearchResults &searchResults, int seq, int fromUin)
{
	disconnect(gadu, SIGNAL(newSearchResults(SearchResults &, int, int)), this,
		SLOT(userCitySearch(SearchResults &, int, int)));

	if (!searchResults.isEmpty())
	{
		const SearchResult& result = searchResults.at(0);
		if (!result.City.isEmpty())
			findCity( result.City );
		else if (!result.FamilyCity.isEmpty())
			findCity(result.FamilyCity);
		else
		{
			close();
			EnterCityDialog *ecd = new EnterCityDialog(user_, cityName_);
			ecd->show();
			ecd->setFocus();
		}
	}
	else
	{
		close();
		EnterCityDialog *ecd = new EnterCityDialog(user_, cityName_);
		ecd->show();
		ecd->setFocus();
	}
}

void SearchingCityDialog::nextServerSearch(const QString &city, const QString &serverName)
{
	progress_->setText(tr("Searching for %1 in %2").arg(city).arg(serverName));
}

void SearchingCityDialog::searchFinished()
{
	const CITYSEARCHRESULTS &results = searchId_.getResult();

	close();
	if (results.isEmpty())
	{
		EnterCityDialog *ecd = new EnterCityDialog(user_, cityName_);
		ecd->show();
		ecd->setFocus();
		MessageBox::msg(tr("City not found!"), false, "Warning");
	}
	else if (results.count() == 1)
	{
		const CitySearchResult &city = results.at(0);
		city.writeUserWeatherData(user_);
		ShowForecastDialog *sfd = new ShowForecastDialog(city);
		sfd->show();
		sfd->setFocus();
	}
	else
	{
		SelectCityDialog *scd = new SelectCityDialog(user_, cityName_, results);
		scd->show();
		scd->setFocus();
	}
}

void SearchingCityDialog::cancelClicked()
{
	kdebugf();

	disconnect(gadu, SIGNAL(newSearchResults(SearchResults &, int, int)), this,
		SLOT(userCitySearch(SearchResults &, int, int)));

	searchId_.cancel();

	close();
	EnterCityDialog *ecd = new EnterCityDialog(user_, cityName_);
	ecd->show();
	ecd->setFocus();

	kdebugf2();
}

/*
	SelectCityDialog
*/

SelectCityDialog::SelectCityDialog(UserListElement user, const QString &cityName, const CITYSEARCHRESULTS &results) :
	QDialog(0, "SelectCityDialog"),
	user_(user),
	cityName_(cityName),
	results_(results)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setCaption(tr("City search"));

	QLabel *cityLabel = new QLabel(tr("Select city:"), this);
	cityList_ = new QListWidget(this);
	cityList_->setMinimumWidth(cityList_->fontMetrics().maxWidth() * 20);
	QWidget * selectButtons = new QWidget(this);
	QHBoxLayout *selectButtonsLayout = new QHBoxLayout(selectButtons);
	selectButtonsLayout->setSpacing(5);
	QPushButton *newSearchButton = new QPushButton(icons_manager->loadIcon("LookupUserInfo"), tr("New search"), selectButtons);
	QPushButton *okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("OK"), selectButtons);
	selectButtonsLayout->addWidget(newSearchButton);
	selectButtonsLayout->addWidget(okButton);
	okButton->setDefault(true);
	QVBoxLayout *selectLayout = new QVBoxLayout(this);
	selectLayout->setSpacing(5);
	selectLayout->setMargin(5);
	selectLayout->addWidget(cityLabel, 0, Qt::AlignLeft);
	selectLayout->addWidget(cityList_, 0);
	selectLayout->addWidget(selectButtons, 0, Qt::AlignCenter);

	CITYSEARCHRESULTS::const_iterator it = results_.begin();
	while (it != results_.end())
	{
		const CitySearchResult &city = *it;

		QString serverName = weather_global->getServerName(city.server_);
		new QListWidgetItem(city.cityName_ + " - " + serverName, cityList_);
		++it;
	}
	cityList_->setCurrentItem(cityList_->item(0));

	connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(newSearchButton, SIGNAL(clicked()), this, SLOT(newSearchClicked()));
	//connect( cityList_, SIGNAL(doubleClicked(QListWidgetItem*)), this, SLOT(showCity(QListWidgetItem*)) );
	//connect( cityList_, SIGNAL(returnPressed(QListWidgetItem*)), this, SLOT(showCity(QListWidgetItem*)) );
}

void SelectCityDialog::okClicked()
{
	showCity(cityList_->currentItem());
}

void SelectCityDialog::newSearchClicked()
{
	close();
	EnterCityDialog *ecd = new EnterCityDialog(user_, cityName_);
	ecd->show();
	ecd->setFocus();
}

void SelectCityDialog::showCity(QListWidgetItem * item)
{
	if (item != 0)
	{
		const CitySearchResult &city = results_[cityList_->row(item)];
		city.writeUserWeatherData(user_);
		close();

		ShowForecastDialog *sfd = new ShowForecastDialog(city);
		sfd->show();
		sfd->setFocus();
	}
}
