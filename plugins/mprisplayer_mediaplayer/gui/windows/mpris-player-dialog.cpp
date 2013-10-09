/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "mpris-player-dialog.h"

MPRISPlayerDialog::MPRISPlayerDialog(bool isEdit, QWidget *parent) :
	QDialog(parent, Qt::Dialog), IsEdit(isEdit)
{
	createGui();
}

MPRISPlayerDialog::~MPRISPlayerDialog()
{

}

void MPRISPlayerDialog::createGui()
{
	PlayerEdit = new QLineEdit(this);
	ServiceEdit = new QLineEdit(this);

	PlayerLabel = new QLabel(tr("Player:"), this);
	ServiceLabel = new QLabel(tr("Service:"), this);

	Buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(Buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(Buttons, SIGNAL(rejected()), this, SLOT(reject()));

	Layout = new QGridLayout(this);

	Layout->addWidget(PlayerLabel, 0, 0);
	Layout->addWidget(PlayerEdit, 0, 1, 1, 5);
	Layout->addWidget(ServiceLabel, 1, 0);
	Layout->addWidget(ServiceEdit, 1, 1, 1, 5);
	Layout->addWidget(Buttons, 2, 5);

	setLayout(Layout);

	if (IsEdit)
		setWindowTitle(tr("Edit Player"));
	else
		setWindowTitle(tr("Add Player"));
}

void MPRISPlayerDialog::setPlayer(const QString &player)
{
	PlayerEdit->setText(player);
}

void MPRISPlayerDialog::setService(const QString &service)
{
	ServiceEdit->setText(service);
}

QString MPRISPlayerDialog::getPlayer()
{
	return PlayerEdit->text();
}

QString MPRISPlayerDialog::getService()
{
	return ServiceEdit->text();
}

#include "moc_mpris-player-dialog.cpp"
