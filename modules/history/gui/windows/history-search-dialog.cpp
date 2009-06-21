/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtCore/QList>
#include <QtGui/QProgressBar>
#include <QtGui/QRadioButton>

#include "debug.h"
#include "emoticons.h"
#include "gui/windows/message-box.h"
#include "icons-manager.h"

#include "history-search-dialog.h"
#include "../../history.h"


HistorySearchDialog::HistorySearchDialog(QWidget *window) : QDialog(window), parentWindow(window)
{
	setWindowTitle(tr("Search in history"));
	setWindowIcon(IconsManager::instance()->loadIcon("History"));
	//nie do ko�ca to pikne
	setGeometry(250, 250, 200, 200);
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(5);
	mainLayout->setSpacing(5);

	QGroupBox *topContainer = new QGroupBox(tr("Find"), this);
	QHBoxLayout* topContainerLay = new QHBoxLayout;
	topContainerLay->setMargin(1);
	topContainerLay->setSpacing(1);

	QWidget* labelAndPhrase = new QWidget;
	QVBoxLayout* labelAndPhraseLay = new QVBoxLayout;
	labelAndPhraseLay->setMargin(2);
	labelAndPhraseLay->setSpacing(3);
// 	QLabel* textLabel = new QLabel(tr("Text to find:"));
// 	labelAndPhraseLay->addWidget(textLabel);
	phraseEdit = new QComboBox;
	phraseEdit->setToolTip(tr("Please type search pattern or select a previous one from the list."));
	phraseEdit->setEditable(true);
	//phraseEdit->addItem("");
	foreach(HistorySearchResult prev, dynamic_cast<HistoryDlg*>(window)->getPreviousSearchResults())
		phraseEdit->addItem(prev.pattern);
	labelAndPhraseLay->addWidget(phraseEdit);
	regExp = new QCheckBox(tr("Regular expression"));
	regExp->setToolTip(tr("Use given pattern as regular expression."));
	labelAndPhraseLay->addWidget(regExp);
	labelAndPhrase->setLayout(labelAndPhraseLay);
	topContainerLay->addWidget(labelAndPhrase);

	
	topContainer->setLayout(topContainerLay);
	mainLayout->addWidget(topContainer);

	QGroupBox *dateContainer = new QGroupBox(tr("Time"), this);
	QHBoxLayout* dateContainerLay = new QHBoxLayout;
	dateContainerLay->setMargin(5);
	dateContainerLay->setSpacing(35);

	QWidget* fromDateWidget = new QWidget;
	QHBoxLayout* fromDateWidgetLay = new QHBoxLayout;
	fromDateWidgetLay->setMargin(1);
	fromDateWidgetLay->setSpacing(1);
	fromDateCheck = new QCheckBox(tr("From"));
	fromDateCombo = new QComboBox;
	fromDateCombo->setToolTip(tr("TODO: write tooltip"));
	fromDateCombo->setEnabled(false);
	connect(fromDateCheck, SIGNAL(toggled(bool)), fromDateCombo, SLOT(setEnabled(bool)));
	fromDateWidgetLay->addWidget(fromDateCheck);
	fromDateWidgetLay->addWidget(fromDateCombo);
	fromDateWidget->setLayout(fromDateWidgetLay);
	dateContainerLay->addWidget(fromDateWidget);

	QWidget* toDateWidget = new QWidget;
	QHBoxLayout* toDateWidgetLay = new QHBoxLayout;
	toDateWidgetLay->setMargin(1);
	toDateWidgetLay->setSpacing(1);
	toDateCheck = new QCheckBox(tr("To"));
	toDateCombo = new QComboBox;
	toDateCombo->setToolTip(tr("TODO: write tooltip"));
	toDateCombo->setEnabled(false);
	connect(toDateCheck, SIGNAL(toggled(bool)), toDateCombo, SLOT(setEnabled(bool)));
	toDateWidgetLay->addWidget(toDateCheck);
	toDateWidgetLay->addWidget(toDateCombo);
	toDateWidget->setLayout(toDateWidgetLay);
	dateContainerLay->addWidget(toDateWidget);

// 	foreach(QDate date, sql_history->getAllDates())
// 	{
// 		fromDateCombo->addItem(date.toString("dd.MM.yyyy"));
// 		toDateCombo->addItem(date.toString("dd.MM.yyyy"));
// 	}
	fromDateCombo->setCurrentIndex(0);
	toDateCombo->setCurrentIndex(toDateCombo->count() - 1);

	dateContainer->setLayout(dateContainerLay);
	mainLayout->addWidget(dateContainer);



	QWidget* middleContainer = new QWidget;
	QHBoxLayout* middleContainerLay = new QHBoxLayout;
	middleContainerLay->setMargin(1);
	middleContainerLay->setSpacing(1);

	QGroupBox *options = new QGroupBox(tr("Options"), this);
	QVBoxLayout* optionsLay = new QVBoxLayout;
	optionsLay->setMargin(1);
	optionsLay->setSpacing(1);
	onlyWholeWords = new QCheckBox(tr("Only whole words"));
	onlyWholeWords->setToolTip(tr("Shows matches only if it is a whole word."));
	optionsLay->addWidget(onlyWholeWords);
	caseSensitive = new QCheckBox(tr("Case sensitive match"));
	caseSensitive->setToolTip(tr("Provide case sensitive search."));
	optionsLay->addWidget(caseSensitive);
///jeszcze obada� czy i jak to si� da... ale pewnie b�dzie ci��ko
// 	QCheckBox* searchWholeHistory = new QCheckBox(tr("Search whole history"));
// 	searchWholeHistory->setToolTip(tr("Search in whole history archive for all contacts and message types."));
// 	optionsLay->addWidget(searchWholeHistory);
	options->setLayout(optionsLay);
	middleContainerLay->addWidget(options);

	QGroupBox *resultOptionsBox = new QGroupBox(tr("Results as:"), this);
	QVBoxLayout* resultOptionsLay = new QVBoxLayout;
	resultOptionsLay->setMargin(3);
	resultOptionsLay->setSpacing(3);
	
	messagesOnly = new QRadioButton(tr("Single messages"), this);
	resultOptionsLay->addWidget(messagesOnly);
	threadToEnd = new QRadioButton(tr("Thread to the end"), this);
	resultOptionsLay->addWidget(threadToEnd);
	wholeThread = new QRadioButton(tr("Whole thread"), this);
	resultOptionsLay->addWidget(wholeThread);
	
	resultOptionsBox->setLayout(resultOptionsLay);
	middleContainerLay->addWidget(resultOptionsBox);

	middleContainer->setLayout(middleContainerLay);
	mainLayout->addWidget(middleContainer);

	QWidget* buttons = new QWidget;
	QHBoxLayout* buttonsLay = new QHBoxLayout;
	buttonsLay->setMargin(1);
	buttonsLay->setSpacing(3);
	QPushButton* goButton = new QPushButton(IconsManager::instance()->loadIcon("LookupUserInfo"), tr("&Find"), this);
	///main->addWidget(findButton);
	connect(goButton, SIGNAL(clicked()), this, SLOT(findButtonClicked()));
	//QLabel* textLabel = new QLabel(tr("Type in here a phrase to find in history."));
	buttonsLay->addWidget(goButton);
	//phraseEdit = new QLineEdit;
	QPushButton* cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CancelWindowButton"), tr("&Cancel"), this);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
	buttonsLay->addWidget(cancelButton);

	buttons->setLayout(buttonsLay);
	mainLayout->addWidget(buttons);


	setLayout(mainLayout);
}

void HistorySearchDialog::findButtonClicked()
{
	kdebugf();
	
	if (!phraseEdit->currentText().isEmpty())
	{
		HistorySearchParameters prm = HistorySearchParameters();
		prm.pattern = phraseEdit->currentText();
		if(fromDateCheck->isChecked())
			prm.fromDate = QDate::fromString(fromDateCombo->currentText(), Qt::ISODate);
		if(toDateCheck->isChecked())
			prm.toDate = QDate::fromString(toDateCombo->currentText(), Qt::ISODate);
		prm.isRegExp = regExp->isChecked();
		prm.isCaseSensv = caseSensitive->isChecked();
		prm.wholeWordsSearch = onlyWholeWords->isChecked();
		if(messagesOnly->isChecked())
			prm.resultsShowMode = 0;
		else if(threadToEnd->isChecked())
			prm.resultsShowMode = 1;
		else if(wholeThread->isChecked())
			prm.resultsShowMode = 2;

		dynamic_cast<HistoryDlg*>(parentWindow)->setSearchParameters(prm);

		accept();
	}
	kdebugf2();
}

void HistorySearchDialog::cancelButtonClicked()
{
	kdebugf();
	//TODO: a mo�e nie zamyka� okna przy starcie wyszukiwania?
	reject();
	kdebugf2();
}
 
