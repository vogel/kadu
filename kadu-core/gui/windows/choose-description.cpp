/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "parser/parser.h"
#include "debug.h"
#include "icons-manager.h"

#include "../modules/gadu_protocol/gadu-protocol.h"

#include "choose-description.h"

QMap<StatusContainer *, ChooseDescription *>ChooseDescription::Dialogs;

void ChooseDescription::show(const Status &status, StatusContainer *statusContainer, const QPoint &position)
{
	ChooseDescription *Dialog = Dialogs[statusContainer];
	if (!Dialog)
	{
		Dialog = new ChooseDescription(statusContainer, Core::instance()->kaduWindow());
		Dialog->setPosition(position);
		Dialogs[statusContainer] = Dialog;
	}

	Dialog->setStatus(status);
	((QDialog *)Dialog)->show();
	Dialog->raise();
}

ChooseDescription::ChooseDescription(StatusContainer *statusContainer, QWidget *parent)
	: QDialog(parent, false), MyStatusContainer(statusContainer)
{
	kdebugf();
	setWindowTitle(tr("Select description"));
	setAttribute(Qt::WA_DeleteOnClose);

// TODO: 0.6.6
// 	while (defaultdescriptions.count() > config_file.readNumEntry("General", "NumberOfDescriptions"))
// 		defaultdescriptions.pop_back();

  	Description = new QComboBox(this);
	Description->setMaxVisibleItems(30);
// 	Description->insertStringList(defaultdescriptions);

	QLineEdit *ss = new QLineEdit(this);
#if 1
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	ss->setMaxLength(gadu->maxDescriptionLength());
#endif
	Description->setLineEdit(ss);

	AvailableChars = new QLabel(this);

	updateAvailableChars(Description->currentText());

	connect(Description, SIGNAL(textChanged(const QString &)), this, SLOT(updateAvailableChars(const QString &)));

	OkButton = new QPushButton(tr("&OK"), this);
	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);

	connect(OkButton, SIGNAL(clicked()), this, SLOT(okPressed()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

	QGridLayout *grid = new QGridLayout(this);

	grid->addWidget(Description, 0, 0, 1, -1);
	grid->addWidget(AvailableChars, 1, 0);
	grid->addWidget(OkButton, 1, 1, Qt::AlignRight);
	grid->addWidget(cancelButton, 1, 2, Qt::AlignRight);

	kdebugf2();
}

ChooseDescription::~ChooseDescription()
{
    	Dialogs.remove(MyStatusContainer);
}

void ChooseDescription::setStatus(const Status &status)
{
	CurrentStatus = status;

	switch (CurrentStatus.type())
	{
		case Status::Online:
			OkButton->setIcon(IconsManager::instance()->loadIcon("OnlineWithDescription"));
			break;
		case Status::Busy:
			OkButton->setIcon(IconsManager::instance()->loadIcon("BusyWithDescription"));
			break;
		case Status::Invisible:
			OkButton->setIcon(IconsManager::instance()->loadIcon("InvisibleWithDescription"));
			break;
		case Status::Offline:
			OkButton->setIcon(IconsManager::instance()->loadIcon("OfflineWithDescription"));
			break;
		default:
			break;
	}
}

void ChooseDescription::setPosition(const QPoint &position)
{
	int width = 250;
	int height = 80;

	QSize sh = sizeHint();

	if (sh.width() > width)
		width = sh.width();

	if (sh.height() > height)
		height = sh.height();

	QDesktopWidget *d = QApplication::desktop();

	QPoint p = position;
	if (p.x() + width + 20 >= d->width())
		p.setX(d->width() - width - 20);
	if (p.y() + height + 20 >= d->height())
		p.setY(d->height() - height - 20);
	move(p);

	resize(width, height);
}

void ChooseDescription::okPressed()
{
	QString description = Description->currentText();
	// TODO: 0.6.6
	Account *account = AccountManager::instance()->defaultAccount();
	//je�eli ju� by� taki opis, to go usuwamy
// 	defaultdescriptions.remove(description);
	//i dodajemy na pocz�tek
// 	defaultdescriptions.prepend(description);

// 	while (defaultdescriptions.count() > config_file.readNumEntry("General", "NumberOfDescriptions"))
// 		defaultdescriptions.pop_back();

	if (config_file.readBoolEntry("General", "ParseStatus", false))
		description = Parser::parse(description, account, Core::instance()->myself(), true);

	CurrentStatus.setDescription(description);
	MyStatusContainer->setStatus(CurrentStatus);

	cancelPressed();
}

void ChooseDescription::cancelPressed()
{
	close();
}

void ChooseDescription::updateAvailableChars(const QString &text)
{
	int length = text.length();

#if 0
	int count = (length - 10) / (gadu->maxDescriptionLength() - 10);
	int rest = (count + 1) * (gadu->maxDescriptionLength() - 10) - length + 10;

	AvailableChars->setText(' ' + QString::number(rest) + " (" + QString::number(count) + ")");
#else
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	AvailableChars->setText(' ' + QString::number(gadu->maxDescriptionLength() - length));
#endif
}
