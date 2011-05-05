/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
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

#ifndef MPRIS_PLAYER_DIALOG_H
#define MPRIS_PLAYER_DIALOG_H

#include <QtGui/QDialog>

class QDialogButtonBox;
class QGridLayout;
class QLabel;
class QLineEdit;

class MPRISPlayerDialog : public QDialog
{
	Q_OBJECT

	QLineEdit *PlayerEdit;
	QLineEdit *ServiceEdit;
	QLabel *PlayerLabel;
	QLabel *ServiceLabel;

	QDialogButtonBox *Buttons;

	QGridLayout *Layout;

	bool IsEdit;

	void createGui();

public:
	explicit MPRISPlayerDialog(bool isEdit = false, QWidget *parent = 0);
	virtual ~MPRISPlayerDialog();

	void setPlayer(const QString &player);
	void setService(const QString &service);
	QString getPlayer();
	QString getService();

};

#endif /* MPRIS_PLAYER_DIALOG_H */
