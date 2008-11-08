/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "account.h"
#include "account_manager.h"
#include "debug.h"
#include "../modules/gadu_protocol/gadu.h"
#include "../modules/gadu_protocol/gadu_search.h"
#include "icons_manager.h"

#include "personal_info.h"

PersonalInfoDialog::PersonalInfoDialog(QWidget *parent)
	: QWidget(parent, Qt::Window),
	le_nickname(0), le_name(0), le_surname(0), cb_gender(0), le_birthyear(0), le_city(0), le_familyname(0),
	le_familycity(0), pb_save(0), State(READY), data(new SearchRecord())
{
	kdebugf();
	setWindowTitle(tr("Personal Information"));
	setAttribute(Qt::WA_DeleteOnClose);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(icons_manager->loadPixmap("PersonalInformationWindowIcon"));


	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addStretch();
	left->setLayout(left_layout);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();

	l_info->setText(tr("This dialog box allows you to manage your personal information, for example your "
				"name, surname or age."));
	l_info->setWordWrap(true);
#ifndef Q_OS_MAC
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif
	// end create main QLabel widgets (icon and app info)

	// our QGroupBox
	QGroupBox *vgb_info = new QGroupBox(tr("Your personal information"));
	QVBoxLayout *info_layout = new QVBoxLayout();
	vgb_info->setLayout(info_layout);
	// end our QGroupBox

	// create our info-fields
	QWidget *nickname = new QWidget;
	QVBoxLayout *nickname_layout = new QVBoxLayout(nickname);

	QWidget *wname = new QWidget;
	QVBoxLayout *name_layout = new QVBoxLayout(wname);

	QWidget *surname = new QWidget;
	QVBoxLayout *surname_layout = new QVBoxLayout(surname);

	le_nickname = new QLineEdit();
	nickname_layout->addWidget(new QLabel(tr("Nickname")));
	nickname_layout->addWidget(le_nickname);

	le_name = new QLineEdit();
	name_layout->addWidget(new QLabel(tr("Name")));
	name_layout->addWidget(le_name);

	le_surname = new QLineEdit();
	surname_layout->addWidget(new QLabel(tr("Surname")));
	surname_layout->addWidget(le_surname);

	QWidget *nicknamesurname = new QWidget;
	QHBoxLayout *nicknamesurname_layout = new QHBoxLayout(nicknamesurname);

	nicknamesurname_layout->addWidget(nickname);
	nicknamesurname_layout->addWidget(wname);
	nicknamesurname_layout->addWidget(surname);
	//
	QWidget *gender = new QWidget;
	QVBoxLayout *gender_layout = new QVBoxLayout(gender);

	QWidget *birthyear = new QWidget;
	QVBoxLayout *birthyear_layout = new QVBoxLayout(birthyear);

	QWidget *city = new QWidget;
	QVBoxLayout *city_layout = new QVBoxLayout(city);

	cb_gender = new QComboBox();
	cb_gender->insertItem(QString::null);
	cb_gender->insertItem(tr("Male"));
	cb_gender->insertItem(tr("Female"));
	cb_gender->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	gender_layout->addWidget(new QLabel(tr("Gender")));
	gender_layout->addWidget(cb_gender);

	le_birthyear = new QLineEdit();
	birthyear_layout->addWidget(new QLabel(tr("Birthyear")));
	birthyear_layout->addWidget(le_birthyear);

	le_city = new QLineEdit();
	city_layout->addWidget(new QLabel(tr("City")));
	city_layout->addWidget(le_city);

	QWidget *genderbirthyearcity = new QWidget;
	QHBoxLayout *genderbirthyearcity_layout = new QHBoxLayout(genderbirthyearcity);

	genderbirthyearcity_layout->addWidget(gender);
	genderbirthyearcity_layout->setStretchFactor(gender, 1);
	genderbirthyearcity_layout->addWidget(birthyear);
	genderbirthyearcity_layout->setStretchFactor(birthyear, 1);
	genderbirthyearcity_layout->addWidget(city);
	genderbirthyearcity_layout->setStretchFactor(city, 1);

	//
	QWidget *fname = new QWidget;
	QVBoxLayout *fname_layout = new QVBoxLayout(fname);

	QWidget *fcity = new QWidget;
	QVBoxLayout *fcity_layout = new QVBoxLayout(fcity);

	le_familyname = new QLineEdit();
	fname_layout->addWidget(new QLabel(tr("Family Name")));
	fname_layout->addWidget(le_familyname);

	le_familycity = new QLineEdit();
	fcity_layout->addWidget(new QLabel(tr("Family City")));
	fcity_layout->addWidget(le_familycity);

	QWidget *fnamefcity = new QWidget;
	QHBoxLayout *fnamefcity_layout = new QHBoxLayout(fnamefcity);

	fnamefcity_layout->addWidget(fname);
	fnamefcity_layout->addWidget(fcity);
	fnamefcity_layout->addStretch();

	info_layout->addWidget(nicknamesurname);
	info_layout->addWidget(genderbirthyearcity);
	info_layout->addWidget(fnamefcity);
	//

	// buttons
	QWidget *bottom = new QWidget;

	pb_save = new QPushButton(icons_manager->loadIcon("SavePersonalInfoButton"), tr("&Save"), bottom, "save");
	connect(pb_save, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));

	QPushButton *pb_reload = new QPushButton(icons_manager->loadIcon("ReloadPersonalInfoButton"), tr("&Reload"), bottom, "save");
	connect(pb_reload, SIGNAL(clicked()), this, SLOT(reloadInfo()));

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout(bottom);
	bottom_layout->addStretch();
	bottom_layout->addWidget(pb_save);
	bottom_layout->addWidget(pb_reload);
	bottom_layout->addWidget(pb_close);

	// end buttons

	QVBoxLayout *center_layout = new QVBoxLayout;
	center_layout->addWidget(l_info);
	center_layout->addWidget(vgb_info);
	center_layout->setStretchFactor(vgb_info, 1);
	center_layout->addWidget(bottom);

	center->setLayout(center_layout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(left);
	layout->addWidget(center);

	setLayout(layout);

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	connect(gadu, SIGNAL(newSearchResults(SearchResults&, int, int)), this, SLOT(fillFields(SearchResults&, int, int)));

	reloadInfo();

//  	loadGeometry(this, "General", "PersonalInfoDialogGeometry", 0, 30, 460, 280);
	kdebugf2();
}

PersonalInfoDialog::~PersonalInfoDialog()
{
	kdebugf();
	delete data;
// 	saveGeometry(this, "General", "PersonalInfoDialogGeometry");
	kdebugf2();
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

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
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

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
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

void PersonalInfoDialog::fillFields(SearchResults &searchResults, int seq, int)
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

