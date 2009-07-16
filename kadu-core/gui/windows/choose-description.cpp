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
#include "status/description-manager.h"
#include "status/description-model.h"

#include "debug.h"
#include "icons-manager.h"

#include "../modules/gadu_protocol/gadu-protocol.h"

#include "choose-description.h"

QMap<StatusContainer *, ChooseDescription *> ChooseDescription::Dialogs;

void ChooseDescription::showDialog(StatusContainer *statusContainer, const QPoint &position)
{
	ChooseDescription *dialog = Dialogs[statusContainer];
	if (!dialog)
	{
		dialog = new ChooseDescription(statusContainer, Core::instance()->kaduWindow());
		dialog->setPosition(position);
		Dialogs[statusContainer] = dialog;
	}

	dialog->show();
	dialog->raise();
}

ChooseDescription::ChooseDescription(StatusContainer *statusContainer, QWidget *parent)
	: QDialog(parent, false), MyStatusContainer(statusContainer)
{
	kdebugf();

	setWindowTitle(tr("Select description"));
	setAttribute(Qt::WA_DeleteOnClose);

	connect(statusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));

	QString currentDescription = MyStatusContainer->status().description();

	Description = new QComboBox(this);
	Description->setMaxVisibleItems(10);
	Description->setModel(DescriptionManager::instance()->model());
	Description->setEditable(true);
	Description->setLineEdit(new QLineEdit(this));
	Description->setInsertPolicy(QComboBox::NoInsert);
	Description->setDuplicatesEnabled(false);

	Description->setEditText(currentDescription);

	AvailableChars = new QLabel(this);

#if 1
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
// 	Description->lineEdit()->setMaxLength(gadu->maxDescriptionLength());
#endif

	connect(Description, SIGNAL(editTextChanged(const QString &)), this, SLOT(currentDescriptionChanged(const QString &)));
	connect(Description, SIGNAL(activated(int)), this, SLOT(activated(int)));
	currentDescriptionChanged(Description->currentText());

	OkButton = new QPushButton(tr("&OK"), this);
	OkButton->setIcon(statusContainer->statusPixmap());
	OkButton->setDefault(true);
	connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QGridLayout *grid = new QGridLayout(this);

	grid->addWidget(Description, 0, 0, 1, -1);
	grid->addWidget(AvailableChars, 1, 0);
	grid->addWidget(OkButton, 1, 1, Qt::AlignRight);
	grid->addWidget(cancelButton, 1, 2, Qt::AlignRight);

	connect(this, SIGNAL(accepted()), this, SLOT(setDescription()));

	kdebugf2();
}

ChooseDescription::~ChooseDescription()
{
	Dialogs.remove(MyStatusContainer);
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

void ChooseDescription::setDescription()
{
	QString description = Description->currentText();
	DescriptionManager::instance()->addDescription(description);

	Account *account = AccountManager::instance()->defaultAccount();

	if (config_file.readBoolEntry("General", "ParseStatus", false))
		description = Parser::parse(description, account, Core::instance()->myself(), true);

	Status status = MyStatusContainer->status();
	status.setDescription(description);
	MyStatusContainer->setStatus(status);
}

void ChooseDescription::activated(int index)
{
	// TODO: fix this workaround
	QString text = Description->model()->data(Description->model()->index(index, 0), Qt::DisplayRole).toString();
	Description->setEditText(text);
}

void ChooseDescription::currentDescriptionChanged(const QString &text)
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

void ChooseDescription::statusChanged()
{
	OkButton->setIcon(MyStatusContainer->statusPixmap());
}
