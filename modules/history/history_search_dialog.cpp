/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "history_search_dialog.h"

#include "config_file.h"
#include "debug.h"
#include "history.h"
#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qhbox.h>
#include <qhgroupbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtooltip.h>
#include <qvbuttongroup.h>

HistoryFindRec::HistoryFindRec() :
	fromdate(), todate(), type(0), data(), reverse(false), actualrecord()
{
}

HistorySearchDialog::HistorySearchDialog(QWidget *parent, UinsList uins) : QDialog(parent),
		from_hgb(0), to_hgb(0), phrase_hgb(0), status_hgb(0), from_chb(0), to_chb(0), reverse_chb(0),
		from_day_cob(0), from_month_cob(0), from_year_cob(0), from_hour_cob(0), from_min_cob(0),
		to_day_cob(0), to_month_cob(0), to_year_cob(0), to_hour_cob(0), to_min_cob(0), status_cob(0),
		phrase_edit(0), criteria_bg(0), phrase_rb(0), status_rb(0), numslist(), uins(uins)
{
	kdebugf();
	setCaption(tr("Search history"));

	int i;
	char buf[128];

	for (i = 0; i <= 59; ++i)
	{
		sprintf(buf, "%02d", i);
		numslist.append(QString(buf));
	}

	QStringList yearslist;
	for (i = 2000; i <= 2020; ++i)
		yearslist.append(QString::number(i));
	QStringList dayslist;
	for (i = 1; i <= 31; ++i)
		dayslist.append(numslist[i]);
	QStringList monthslist;
	for (i = 1; i <= 12; ++i)
		monthslist.append(numslist[i]);
	QStringList hourslist;
	for (i = 0; i <= 23; ++i)
		hourslist.append(numslist[i]);
	QStringList minslist;
	for (i = 0; i <= 59; ++i)
		minslist.append(numslist[i]);

	QHBox *from_hb = new QHBox(this);
	from_chb = new QCheckBox(tr("&From:") ,from_hb);
	from_hgb = new QHGroupBox(from_hb);
	from_day_cob = new QComboBox(from_hgb);
	from_day_cob->insertStringList(dayslist);
	QToolTip::add(from_day_cob, tr("day"));
	from_month_cob = new QComboBox(from_hgb);
	from_month_cob->insertStringList(monthslist);
	QToolTip::add(from_month_cob, tr("month"));
	from_year_cob = new QComboBox(from_hgb);
	from_year_cob->insertStringList(yearslist);
	QToolTip::add(from_year_cob, tr("year"));
	from_hour_cob = new QComboBox(from_hgb);
	from_hour_cob->insertStringList(hourslist);
	QToolTip::add(from_hour_cob, tr("hour"));
	from_min_cob = new QComboBox(from_hgb);
	from_min_cob->insertStringList(minslist);
	QToolTip::add(from_min_cob, tr("minute"));

	QHBox *to_hb = new QHBox(this);
	to_chb = new QCheckBox(tr("&To:") ,to_hb);
	to_hgb = new QHGroupBox(to_hb);
	to_day_cob = new QComboBox(to_hgb);
	to_day_cob->insertStringList(dayslist);
	QToolTip::add(to_day_cob, tr("day"));
	to_month_cob = new QComboBox(to_hgb);
	to_month_cob->insertStringList(monthslist);
	QToolTip::add(to_month_cob, tr("month"));
	to_year_cob = new QComboBox(to_hgb);
	to_year_cob->insertStringList(yearslist);
	QToolTip::add(to_year_cob, tr("year"));
	to_hour_cob = new QComboBox(to_hgb);
	to_hour_cob->insertStringList(hourslist);
	QToolTip::add(to_hour_cob, tr("hour"));
	to_min_cob = new QComboBox(to_hgb);
	to_min_cob->insertStringList(minslist);
	QToolTip::add(to_min_cob, tr("minute"));

	criteria_bg = new QVButtonGroup(tr("Find Criteria"), this);
	phrase_rb = new QRadioButton(tr("&Pattern"), criteria_bg);
	status_rb = new QRadioButton(tr("&Status"), criteria_bg);
	if (config_file.readBoolEntry("History", "DontShowStatusChanges"))
		status_rb->setEnabled(false);
	criteria_bg->insert(phrase_rb, 1);
	criteria_bg->insert(status_rb, 2);

	phrase_hgb = new QHGroupBox(tr("Pattern"), this);
	phrase_edit = new QLineEdit(phrase_hgb);
	status_hgb = new QHGroupBox(tr("Status"), this);
	status_cob = new QComboBox(status_hgb);
	for (i = 0; i < 4; ++i)
		status_cob->insertItem(qApp->translate("@default", UserStatus::name(i * 2).ascii()));

	reverse_chb = new QCheckBox(tr("&Reverse find"), this);

	QPushButton *find_btn = new QPushButton(tr("&Find"), this);
	QPushButton *reset_btn = new QPushButton(tr("Reset"), this);
	QPushButton *cancel_btn = new QPushButton(tr("&Cancel"), this);

	connect(from_chb, SIGNAL(toggled(bool)), this, SLOT(fromToggled(bool)));
	connect(from_month_cob, SIGNAL(activated(int)), this, SLOT(correctFromDays(int)));
	connect(to_chb, SIGNAL(toggled(bool)), this, SLOT(toToggled(bool)));
	connect(to_month_cob, SIGNAL(activated(int)), this, SLOT(correctToDays(int)));
	connect(criteria_bg, SIGNAL(clicked(int)), this, SLOT(criteriaChanged(int)));
	connect(find_btn, SIGNAL(clicked()), this, SLOT(findBtnClicked()));
	connect(reset_btn, SIGNAL(clicked()), this, SLOT(resetBtnClicked()));
	connect(cancel_btn, SIGNAL(clicked()), this, SLOT(cancelBtnClicked()));

	QGridLayout *grid = new QGridLayout(this, 6, 4, 5, 5);
	grid->addMultiCellWidget(from_hb, 0, 0, 0, 3);
	grid->addMultiCellWidget(to_hb, 1, 1, 0, 3);
	grid->addMultiCellWidget(criteria_bg, 2, 3, 0, 1);
	grid->addMultiCellWidget(phrase_hgb, 2, 2, 2, 3);
	grid->addMultiCellWidget(status_hgb, 3, 3, 2, 3);
	grid->addMultiCellWidget(reverse_chb, 4, 4, 0, 3, Qt::AlignLeft);
	grid->addWidget(find_btn, 5, 1);
	grid->addWidget(reset_btn, 5, 2);
	grid->addWidget(cancel_btn, 5, 3);

	phrase_edit->setFocus();
	kdebugf2();
}

static const int daysForMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void HistorySearchDialog::correctFromDays(int index)
{
	kdebugf();
	if (daysForMonth[index] != from_day_cob->count())
	{
		QStringList dayslist;
		for (int i = 1; i <= daysForMonth[index]; ++i)
			dayslist.append(numslist[i]);
		int current_day = from_day_cob->currentItem();
		from_day_cob->clear();
		from_day_cob->insertStringList(dayslist);
		if (current_day <= from_day_cob->count())
			from_day_cob->setCurrentItem(current_day);
	}
	kdebugf2();
}

void HistorySearchDialog::correctToDays(int index)
{
	kdebugf();
	if (daysForMonth[index] != to_day_cob->count())
	{
		QStringList dayslist;
		for (int i = 1; i <= daysForMonth[index]; ++i)
			dayslist.append(numslist[i]);
		int current_day = to_day_cob->currentItem();
		to_day_cob->clear();
		to_day_cob->insertStringList(dayslist);
		if (current_day <= to_day_cob->count())
			to_day_cob->setCurrentItem(current_day);
	}
	kdebugf2();
}

void HistorySearchDialog::fromToggled(bool on)
{
	from_hgb->setEnabled(on);
}

void HistorySearchDialog::toToggled(bool on)
{
	to_hgb->setEnabled(on);
}

void HistorySearchDialog::criteriaChanged(int id)
{
	phrase_hgb->setEnabled(id == 1);
	status_hgb->setEnabled(id != 1);
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
	QValueList<HistoryEntry> entries;

	entries = history->getHistoryEntries(uins, 0, 1);
	if (!entries.isEmpty())
	{
		from_day_cob->setCurrentItem(entries[0].date.date().day() - 1);
		from_month_cob->setCurrentItem(entries[0].date.date().month() - 1);
		from_year_cob->setCurrentItem(entries[0].date.date().year() - 2000);
		from_hour_cob->setCurrentItem(entries[0].date.time().hour());
		from_min_cob->setCurrentItem(entries[0].date.time().minute());
		correctFromDays(entries[0].date.date().month() - 1);
	}
	kdebugf2();
}

void HistorySearchDialog::resetToDate()
{
	kdebugf();
	QValueList<HistoryEntry> entries;

	entries = history->getHistoryEntries(uins, history->getHistoryEntriesCount(uins) - 1, 1);
	if (!entries.isEmpty())
	{
		to_day_cob->setCurrentItem(entries[0].date.date().day() - 1);
		to_month_cob->setCurrentItem(entries[0].date.date().month() - 1);
		to_year_cob->setCurrentItem(entries[0].date.date().year() - 2000);
		to_hour_cob->setCurrentItem(entries[0].date.time().hour());
		to_min_cob->setCurrentItem(entries[0].date.time().minute());
		correctToDays(entries[0].date.date().month() - 1);
	}
	kdebugf2();
}

void HistorySearchDialog::resetBtnClicked()
{
	kdebugf();
	from_hgb->setEnabled(false);
	from_chb->setChecked(false);
	resetFromDate();
	to_chb->setChecked(false);
	to_hgb->setEnabled(false);
	resetToDate();
	criteria_bg->setButton(1);
	phrase_edit->text().truncate(0);
	status_cob->setCurrentItem(0);
	criteriaChanged(1);
	reverse_chb->setChecked(false);
	kdebugf2();
}

void HistorySearchDialog::setDialogValues(HistoryFindRec &findrec)
{
	kdebugf();
	from_chb->setChecked(!findrec.fromdate.isNull());
	from_hgb->setEnabled(!findrec.fromdate.isNull());
	if (findrec.fromdate.isNull())
		resetFromDate();
	else
	{
		from_day_cob->setCurrentItem(findrec.fromdate.date().day() - 1);
		from_month_cob->setCurrentItem(findrec.fromdate.date().month() - 1);
		from_year_cob->setCurrentItem(findrec.fromdate.date().year() - 2000);
		from_hour_cob->setCurrentItem(findrec.fromdate.time().hour());
		from_min_cob->setCurrentItem(findrec.fromdate.time().minute());
		correctFromDays(findrec.fromdate.date().month() - 1);
	}
	to_chb->setChecked(!findrec.todate.isNull());
	to_hgb->setEnabled(!findrec.todate.isNull());
	if (findrec.todate.isNull())
		resetToDate();
	else
	{
		to_day_cob->setCurrentItem(findrec.todate.date().day() - 1);
		to_month_cob->setCurrentItem(findrec.todate.date().month() - 1);
		to_year_cob->setCurrentItem(findrec.todate.date().year() - 2000);
		to_hour_cob->setCurrentItem(findrec.todate.time().hour());
		to_min_cob->setCurrentItem(findrec.todate.time().minute());
		correctToDays(findrec.todate.date().month() - 1);
	}
	criteria_bg->setButton(findrec.type);
	criteriaChanged(findrec.type);
	switch (findrec.type)
	{
		case 1:
			phrase_edit->setText(findrec.data);
			break;
		case 2:
		{
			int status=0;
			if (findrec.data == "avail")
				status = 0;
			else if (findrec.data == "busy")
				status = 1;
			else if (findrec.data == "invisible")
				status = 2;
			else if (findrec.data == "notavail")
				status = 3;
			status_cob->setCurrentItem(status);
			break;
		}
	}
	reverse_chb->setChecked(findrec.reverse);
	kdebugf2();
}

HistoryFindRec HistorySearchDialog::getDialogValues() const
{
	kdebugf();
	HistoryFindRec findrec;

	if (from_chb->isChecked())
	{
		findrec.fromdate.setDate(QDate(from_year_cob->currentItem() + 2000,
			from_month_cob->currentItem() + 1, from_day_cob->currentItem() + 1));
		findrec.fromdate.setTime(QTime(from_hour_cob->currentItem(), from_min_cob->currentItem()));
	}
	if (to_chb->isChecked())
	{
		findrec.todate.setDate(QDate(to_year_cob->currentItem() + 2000,
			to_month_cob->currentItem() + 1, to_day_cob->currentItem() + 1));
		findrec.todate.setTime(QTime(to_hour_cob->currentItem(), to_min_cob->currentItem()));
	}
	findrec.type = criteria_bg->id(criteria_bg->selected());
	switch (findrec.type)
	{
		case 1:
			findrec.data = phrase_edit->text();
			break;
		case 2:
			switch (status_cob->currentItem())
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
			}
			break;
	}
	findrec.reverse = reverse_chb->isChecked();
	kdebugf2();
	return findrec;
}
