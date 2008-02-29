/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <q3vbox.h>
#include <q3vgroupbox.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QKeyEvent>

#include "debug.h"
#include "icons_manager.h"
#include "misc.h"
#include "personal_info.h"

PersonalInfoDialog::PersonalInfoDialog(QWidget *parent, const char *name) : Q3HBox(parent, name, Qt::WType_TopLevel | Qt::WDestructiveClose),
	le_nickname(0), le_name(0), le_surname(0), cb_gender(0), le_birthyear(0), le_city(0), le_familyname(0),
	le_familycity(0), pb_save(0), State(READY), data(new SearchRecord()), layoutHelper(new LayoutHelper())
{
	kdebugf();
	setCaption(tr("Personal Information"));
	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	Q3VBox *left = new Q3VBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	QWidget *blank = new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	Q3VBox *center = new Q3VBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager->loadPixmap("PersonalInformationWindowIcon"));
	l_info->setText(tr("This dialog box allows you to manage your personal information, for example your "
				"name, surname or age."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	// our QVGroupBox
	Q3VGroupBox *vgb_info = new Q3VGroupBox(center);
	vgb_info->setTitle(tr("Your personal information"));
	center->setStretchFactor(vgb_info, 1);
	// end our QGroupBox

	// create our info-fields
	Q3HBox *hb_nicknamesurname = new Q3HBox(vgb_info);
	Q3VBox *vb_nickname = new Q3VBox(hb_nicknamesurname);
	Q3VBox *vb_name = new Q3VBox(hb_nicknamesurname);
	Q3VBox *vb_surname = new Q3VBox(hb_nicknamesurname);
	hb_nicknamesurname->setSpacing(3);
	vb_nickname->setSpacing(3);
	vb_name->setSpacing(3);
	vb_surname->setSpacing(3);

	new QLabel(tr("Nickname"), vb_nickname);
	le_nickname = new QLineEdit(vb_nickname);
	new QLabel(tr("Name"), vb_name);
	le_name = new QLineEdit(vb_name);
	new QLabel(tr("Surname"), vb_surname);
	le_surname = new QLineEdit(vb_surname);

	//
	Q3HBox *hb_genderbirthyearcity = new Q3HBox(vgb_info);
	Q3VBox *vb_gender = new Q3VBox(hb_genderbirthyearcity);
	Q3VBox *vb_birthyear = new Q3VBox(hb_genderbirthyearcity);
	Q3VBox *vb_city = new Q3VBox(hb_genderbirthyearcity);
	hb_genderbirthyearcity->setSpacing(3);
	vb_gender->setSpacing(3);
	vb_birthyear->setSpacing(3);
	vb_city->setSpacing(3);

	new QLabel(tr("Gender"), vb_gender);
	cb_gender = new QComboBox(vb_gender);
	cb_gender->insertItem(QString::null);
	cb_gender->insertItem(tr("Male"));
	cb_gender->insertItem(tr("Female"));
	cb_gender->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	hb_genderbirthyearcity->setStretchFactor(cb_gender, 1);
	new QLabel(tr("Birthyear"), vb_birthyear);
	le_birthyear = new QLineEdit(vb_birthyear);
	hb_genderbirthyearcity->setStretchFactor(le_birthyear, 1);
	new QLabel(tr("City"), vb_city);
	le_city = new QLineEdit(vb_city);
	hb_genderbirthyearcity->setStretchFactor(le_city, 1);

	//
	Q3HBox *hb_fnamefcity = new Q3HBox(vgb_info);
	Q3VBox *vb_fname = new Q3VBox(hb_fnamefcity);
	Q3VBox *vb_fcity = new Q3VBox(hb_fnamefcity);
	Q3VBox *vb_blank = new Q3VBox(hb_fnamefcity);
	hb_fnamefcity->setSpacing(3);
	vb_fname->setSpacing(3);
	vb_fcity->setSpacing(3);
	vb_blank->setSpacing(3);

	new QLabel(tr("Family Name"), vb_fname);
	le_familyname = new QLineEdit(vb_fname);
	new QLabel(tr("Family City"), vb_fcity);
	le_familycity = new QLineEdit(vb_fcity);
	QWidget *w_blankfill = new QWidget(vb_blank);
	w_blankfill->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	//

	// buttons
	Q3HBox *bottom=new Q3HBox(center);
	QWidget *blank2=new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	pb_save = new QPushButton(icons_manager->loadIcon("SavePersonalInfoButton"), tr("&Save"), bottom, "save");
	QPushButton *pb_reload = new QPushButton(icons_manager->loadIcon("ReloadPersonalInfoButton"), tr("&Reload"), bottom, "save");
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_save, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
	connect(pb_reload, SIGNAL(clicked()), this, SLOT(reloadInfo()));

	connect(gadu, SIGNAL(newSearchResults(SearchResults&, int, int)), this, SLOT(fillFields(SearchResults&, int, int)));

	reloadInfo();

	layoutHelper->addLabel(l_info);

 	loadGeometry(this, "General", "PersonalInfoDialogGeometry", 0, 30, 460, 280);
	kdebugf2();
}

PersonalInfoDialog::~PersonalInfoDialog()
{
	kdebugf();
	delete data;
// 	saveGeometry(this, "General", "PersonalInfoDialogGeometry");
	delete layoutHelper;
	kdebugf2();
}

void PersonalInfoDialog::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}

void PersonalInfoDialog::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
	if ((ke_event->key() == Qt::Key_Return || ke_event->key() == Qt::Key_Enter))
		saveButtonClicked();
}

void PersonalInfoDialog::reloadInfo()
{
	kdebugf();
	if (!gadu->currentStatus().isOffline())
	{
		State = READING;
		gadu->getPersonalInfo(*data);
	}
	else
		pb_save->setEnabled(false);
	kdebugf2();
}

void PersonalInfoDialog::saveButtonClicked()
{
	kdebugf();

	if (gadu->currentStatus().isOffline())
		return;

	SearchResult save;

	State = WRITING;
	save.First = le_name->text();
	save.Last = le_surname->text();
	save.Nick = le_nickname->text();
	save.City = le_city->text();
	save.Born = le_birthyear->text();
	save.Gender = cb_gender->currentItem();
	save.FamilyName = le_familyname->text();
	save.FamilyCity = le_familycity->text();
	gadu->setPersonalInfo(*data, save);

	setEnabled(false);

	kdebugf2();
}

void PersonalInfoDialog::fillFields(SearchResults& searchResults, int seq, int)
{
	kdebugf();

	if (data->Seq != seq)
		return;

	SearchResult result;

	switch (State)
	{

		case READING:
			kdebugmf(KDEBUG_INFO, "Done reading info,\n");
			if (searchResults.isEmpty())
			{
				State = READY;
				break;
			}

			result = searchResults[0];
			le_name->setText(result.First);
			le_surname->setText(result.Last);
			le_nickname->setText(result.Nick);
			le_birthyear->setText(result.Born);
			le_city->setText(result.City);
			le_familyname->setText(result.FamilyName);
			le_familycity->setText(result.FamilyCity);
			cb_gender->setCurrentItem(result.Gender);
			State = READY;
			break;

		case WRITING:
			kdebugmf(KDEBUG_INFO, "Done writing info.\n");
			State = READY;
			break;

		default:
			break;
	}

	setEnabled(true);
	kdebugf2();
}

