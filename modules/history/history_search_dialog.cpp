/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "config_file.h"
#include "debug.h"
#include "history.h"

#include "history_search_dialog.h"

HistoryFindRec::HistoryFindRec() :
	fromdate(), todate(), type(0), data(), reverse(false), actualrecord()
{
}

HistorySearchDialog::HistorySearchDialog(QWidget *parent, UinsList uins)
	: QDialog(parent),
		fromGroupBox(0), toGroupBox(0), phraseGroupBox(0), statusGroupBox(0), fromCheckBox(0), toCheckBox(0), reverseCheckBox(0),
		fromDayComboBox(0), fromMonthComboBox(0), fromYearComboBox(0), fromHourComboBox(0), fromMinComboBox(0),
		toDayComboBox(0), toMonthComboBox(0), toYearComboBox(0), toHourComboBox(0), toMinComboBox(0), statusComboBox(0),
		phraseEdit(0), criteriaButtonGroup(0), phraseRadioButton (0), statusRadioButton (0), numsList(), uins(uins)
{
	kdebugf();

	setWindowTitle(tr("Search history"));

	int i;
	char buf[128];

	for (i = 0; i <= 59; ++i)
	{
		sprintf(buf, "%02d", i);
		numsList.append(QString(buf));
	}

	QStringList yearsList;
	for (i = 2000; i <= 2020; ++i)
		yearsList.append(QString::number(i));
	QStringList daysList;
	for (i = 1; i <= 31; ++i)
		daysList.append(numsList[i]);
	QStringList monthsList;
	for (i = 1; i <= 12; ++i)
		monthsList.append(numsList[i]);
	QStringList hoursList;
	for (i = 0; i <= 23; ++i)
		hoursList.append(numsList[i]);
	QStringList minsList;
	for (i = 0; i <= 59; ++i)
		minsList.append(numsList[i]);

	QWidget *fromWidget = new QWidget(this);
	QHBoxLayout *fromLayout = new QHBoxLayout(fromWidget);
	
	fromCheckBox = new QCheckBox(tr("&From:"), fromWidget);
	fromGroupBox = new QGroupBox(fromWidget);
	QHBoxLayout *fromGroupBoxLayout = new QHBoxLayout(fromGroupBox);

	fromDayComboBox = new QComboBox(fromGroupBox);
	fromDayComboBox->insertStringList(daysList);
	fromDayComboBox->setToolTip(tr("day"));

	fromMonthComboBox = new QComboBox(fromGroupBox);
	fromMonthComboBox->insertStringList(monthsList);
	fromMonthComboBox->setToolTip(tr("month"));

	fromYearComboBox = new QComboBox(fromGroupBox);
	fromYearComboBox->insertStringList(yearsList);
	fromYearComboBox->setToolTip(tr("year"));

	fromHourComboBox = new QComboBox(fromGroupBox);
	fromHourComboBox->insertStringList(hoursList);
	fromHourComboBox->setToolTip(tr("hour"));

	fromMinComboBox = new QComboBox(fromGroupBox);
	fromMinComboBox->insertStringList(minsList);
	fromMinComboBox->setToolTip(tr("minute"));

	fromGroupBoxLayout->addWidget(fromDayComboBox);
	fromGroupBoxLayout->addWidget(fromMonthComboBox);
	fromGroupBoxLayout->addWidget(fromYearComboBox);
	fromGroupBoxLayout->addWidget(fromHourComboBox);
	fromGroupBoxLayout->addWidget(fromMinComboBox);

	fromLayout->addWidget(fromCheckBox);
	fromLayout->addWidget(fromGroupBox);

	QWidget *toWidget = new QWidget(this);
	QHBoxLayout *toLayout = new QHBoxLayout(toWidget);
	
	toCheckBox = new QCheckBox(tr("&From:"), toWidget);
	toGroupBox = new QGroupBox(toWidget);
	QHBoxLayout *toGroupBoxLayout = new QHBoxLayout(toGroupBox);

	toDayComboBox = new QComboBox(toGroupBox);
	toDayComboBox->insertStringList(daysList);
	toDayComboBox->setToolTip(tr("day"));

	toMonthComboBox = new QComboBox(toGroupBox);
	toMonthComboBox->insertStringList(monthsList);
	toMonthComboBox->setToolTip(tr("month"));

	toYearComboBox = new QComboBox(toGroupBox);
	toYearComboBox->insertStringList(yearsList);
	toYearComboBox->setToolTip(tr("year"));

	toHourComboBox = new QComboBox(toGroupBox);
	toHourComboBox->insertStringList(hoursList);
	toHourComboBox->setToolTip(tr("hour"));

	toMinComboBox = new QComboBox(toGroupBox);
	toMinComboBox->insertStringList(minsList);
	toMinComboBox->setToolTip(tr("minute"));

	toGroupBoxLayout->addWidget(toDayComboBox);
	toGroupBoxLayout->addWidget(toMonthComboBox);
	toGroupBoxLayout->addWidget(toYearComboBox);
	toGroupBoxLayout->addWidget(toHourComboBox);
	toGroupBoxLayout->addWidget(toMinComboBox);

	toLayout->addWidget(toCheckBox);
	toLayout->addWidget(toGroupBox);

	criteriaButtonGroup = new QButtonGroup(this);
	criteriaGroupBox = new QGroupBox(tr("Find Criteria"), this);
	QHBoxLayout *criteriaLayout = new QHBoxLayout(criteriaGroupBox);

	phraseRadioButton  = new QRadioButton(tr("&Pattern"), criteriaGroupBox);
	statusRadioButton  = new QRadioButton(tr("&Status"), criteriaGroupBox);

	if (config_file.readBoolEntry("History", "ShowStatusChanges"))
		statusRadioButton->setEnabled(true);

	criteriaButtonGroup->addButton(phraseRadioButton , 1);
	criteriaButtonGroup->addButton(statusRadioButton , 2);

	criteriaLayout->addWidget(phraseRadioButton);
	criteriaLayout->addWidget(statusRadioButton);

	phraseGroupBox = new QGroupBox(tr("Pattern"), this);
	QHBoxLayout *phraseLayout = new QHBoxLayout(phraseGroupBox);

	phraseEdit = new QLineEdit(phraseGroupBox);

	phraseLayout->addWidget(phraseEdit);

	statusGroupBox = new QGroupBox(tr("Status"), this);
	QHBoxLayout *statusLayout = new QHBoxLayout(statusGroupBox);
	statusComboBox = new QComboBox(statusGroupBox);

	for (i = 0; i < 6; ++i)
		statusComboBox->insertItem(i, qApp->translate("@default", UserStatus::name(i * 2).ascii()));
	statusLayout->addWidget(statusComboBox);

	reverseCheckBox = new QCheckBox(tr("&Reverse find"), this);

	QPushButton *findButton = new QPushButton(tr("&Find"), this);
	QPushButton *resetButton = new QPushButton(tr("Reset"), this);
	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);

	connect(fromCheckBox, SIGNAL(toggled(bool)), this, SLOT(fromToggled(bool)));
	connect(fromMonthComboBox, SIGNAL(activated(int)), this, SLOT(correctFromDays(int)));
	connect(toCheckBox, SIGNAL(toggled(bool)), this, SLOT(toToggled(bool)));
	connect(toMonthComboBox, SIGNAL(activated(int)), this, SLOT(correctToDays(int)));
	connect(criteriaButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(criteriaChanged(int)));
	connect(findButton, SIGNAL(clicked()), this, SLOT(findBtnClicked()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetBtnClicked()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelBtnClicked()));

	QGridLayout *grid = new QGridLayout(this);
	grid->setMargin(0);
	grid->setSpacing(0);
	grid->addMultiCellWidget(fromWidget, 0, 0, 0, 3);
	grid->addMultiCellWidget(toWidget, 1, 1, 0, 3);
	grid->addMultiCellWidget(criteriaGroupBox, 2, 3, 0, 1);
	grid->addMultiCellWidget(phraseGroupBox, 2, 2, 2, 3);
	grid->addMultiCellWidget(statusGroupBox, 3, 3, 2, 3);
	grid->addMultiCellWidget(reverseCheckBox, 4, 4, 0, 3, Qt::AlignLeft);
	grid->addWidget(findButton, 5, 1);
	grid->addWidget(resetButton, 5, 2);
	grid->addWidget(cancelButton, 5, 3);

	phraseEdit->setFocus();
	kdebugf2();
}

static const int daysForMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void HistorySearchDialog::correctFromDays(int index)
{
	kdebugf();
	if (daysForMonth[index] != fromDayComboBox->count())
	{
		QStringList daysList;
		for (int i = 1; i <= daysForMonth[index]; ++i)
			daysList.append(numsList[i]);

		int current_day = fromDayComboBox->currentIndex();
		fromDayComboBox->clear();
		fromDayComboBox->insertStringList(daysList);

		if (current_day <= fromDayComboBox->count())
			fromDayComboBox->setCurrentIndex(current_day);
	}
	kdebugf2();
}

void HistorySearchDialog::correctToDays(int index)
{
	kdebugf();
	if (daysForMonth[index] != toDayComboBox->count())
	{
		QStringList daysList;
		for (int i = 1; i <= daysForMonth[index]; ++i)
			daysList.append(numsList[i]);
		int current_day = toDayComboBox->currentIndex();
		toDayComboBox->clear();
		toDayComboBox->insertStringList(daysList);
		if (current_day <= toDayComboBox->count())
			toDayComboBox->setCurrentIndex(current_day);
	}
	kdebugf2();
}

void HistorySearchDialog::fromToggled(bool on)
{
	fromGroupBox->setEnabled(on);
}

void HistorySearchDialog::toToggled(bool on)
{
	toGroupBox->setEnabled(on);
}

void HistorySearchDialog::criteriaChanged(int id)
{
	phraseGroupBox->setEnabled(id == 1);
	statusGroupBox->setEnabled(id != 1);
}

void HistorySearchDialog::findBtnClicked()
{
	accept();
}

void HistorySearchDialog::cancelBtnClicked()
{
	reject();
}

void HistorySearchDialog::resetFromDate()
{
	kdebugf();
	QList<HistoryEntry> entries;

	entries = history->getHistoryEntries(uins, 0, 1);
	if (!entries.isEmpty())
	{
		fromDayComboBox->setCurrentIndex(entries[0].date.date().day() - 1);
		fromMonthComboBox->setCurrentIndex(entries[0].date.date().month() - 1);
		fromYearComboBox->setCurrentIndex(entries[0].date.date().year() - 2000);
		fromHourComboBox->setCurrentIndex(entries[0].date.time().hour());
		fromMinComboBox->setCurrentIndex(entries[0].date.time().minute());
		correctFromDays(entries[0].date.date().month() - 1);
	}
	kdebugf2();
}

void HistorySearchDialog::resetToDate()
{
	kdebugf();
	QList<HistoryEntry> entries;

	entries = history->getHistoryEntries(uins, history->getHistoryEntriesCount(uins) - 1, 1);
	if (!entries.isEmpty())
	{
		toDayComboBox->setCurrentIndex(entries[0].date.date().day() - 1);
		toMonthComboBox->setCurrentIndex(entries[0].date.date().month() - 1);
		toYearComboBox->setCurrentIndex(entries[0].date.date().year() - 2000);
		toHourComboBox->setCurrentIndex(entries[0].date.time().hour());
		toMinComboBox->setCurrentIndex(entries[0].date.time().minute());
		correctToDays(entries[0].date.date().month() - 1);
	}
	kdebugf2();
}

void HistorySearchDialog::resetBtnClicked()
{
	kdebugf();
	fromGroupBox->setEnabled(false);
	fromCheckBox->setChecked(false);
	resetFromDate();
	toCheckBox->setChecked(false);
	toGroupBox->setEnabled(false);
	resetToDate();
	criteriaButtonGroup->button(1)->setChecked(true);
	phraseEdit->text().truncate(0);
	statusComboBox->setCurrentIndex(0);
	criteriaChanged(1);
	reverseCheckBox->setChecked(false);
	kdebugf2();
}

void HistorySearchDialog::setDialogValues(HistoryFindRec &findrec, bool enable)
{
	kdebugf();
	fromCheckBox->setChecked(!findrec.fromdate.isNull() && enable);
	fromGroupBox->setEnabled(!findrec.fromdate.isNull() && enable);
	if (findrec.fromdate.isNull())
		resetFromDate();
	else
	{
		fromDayComboBox->setCurrentIndex(findrec.fromdate.date().day() - 1);
		fromMonthComboBox->setCurrentIndex(findrec.fromdate.date().month() - 1);
		fromYearComboBox->setCurrentIndex(findrec.fromdate.date().year() - 2000);
		fromHourComboBox->setCurrentIndex(findrec.fromdate.time().hour());
		fromMinComboBox->setCurrentIndex(findrec.fromdate.time().minute());
		correctFromDays(findrec.fromdate.date().month() - 1);
	}
	toCheckBox->setChecked(!findrec.todate.isNull() && enable);
	toGroupBox->setEnabled(!findrec.todate.isNull() && enable);

	if (findrec.todate.isNull())
		resetToDate();
	else
	{
		toDayComboBox->setCurrentIndex(findrec.todate.date().day() - 1);
		toMonthComboBox->setCurrentIndex(findrec.todate.date().month() - 1);
		toYearComboBox->setCurrentIndex(findrec.todate.date().year() - 2000);
		toHourComboBox->setCurrentIndex(findrec.todate.time().hour());
		toMinComboBox->setCurrentIndex(findrec.todate.time().minute());
		correctToDays(findrec.todate.date().month() - 1);
	}
	criteriaButtonGroup->button(findrec.type)->setChecked(true);
	criteriaChanged(findrec.type);

	switch (findrec.type)
	{
		case 1:
			phraseEdit->setText(findrec.data);
			break;
		case 2:
		{
			int status = 0;
			if (findrec.data == "avail")
				status = 0;
			else if (findrec.data == "busy")
				status = 1;
			else if (findrec.data == "invisible")
				status = 2;
			else if (findrec.data == "notavail")
				status = 3;
			else if (findrec.data == "ffc")
				status = 4;
			else if (findrec.data == "dnd")
				status = 5;
			statusComboBox->setCurrentIndex(status);
			break;
		}
	}
	reverseCheckBox->setChecked(findrec.reverse);
	kdebugf2();
}

HistoryFindRec HistorySearchDialog::getDialogValues() const
{
	kdebugf();
	HistoryFindRec findrec;
	findrec.actualrecord = 0;

	if (fromCheckBox->isChecked())
	{
		findrec.fromdate.setDate(QDate(fromYearComboBox->currentIndex() + 2000,
			fromMonthComboBox->currentIndex() + 1, fromDayComboBox->currentIndex() + 1));
		findrec.fromdate.setTime(QTime(fromHourComboBox->currentIndex(), fromMinComboBox->currentIndex()));
	}
	if (toCheckBox->isChecked())
	{
		findrec.todate.setDate(QDate(toYearComboBox->currentIndex() + 2000,
			toMonthComboBox->currentIndex() + 1, toDayComboBox->currentIndex() + 1));
		findrec.todate.setTime(QTime(toHourComboBox->currentIndex(), toMinComboBox->currentIndex()));
	}
	findrec.type = criteriaButtonGroup->id(criteriaButtonGroup->checkedButton());

	switch (findrec.type)
	{
		case 1:
			findrec.data = phraseEdit->text();
			break;
		case 2:
			switch (statusComboBox->currentIndex())
			{
				case 0:
					findrec.data = "avail";
					break;
				case 1:
					findrec.data = "busy";
					break;
				case 2:
					findrec.data = "invisible";
					break;
				case 3:
					findrec.data = "notavail";
					break;
				case 4:
					findrec.data = "ffc";
					break;
				case 5:
					findrec.data = "dnd";
					break;
			}
			break;
	}
	findrec.reverse = reverseCheckBox->isChecked();
	kdebugf2();
	return findrec;
}
