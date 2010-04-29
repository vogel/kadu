/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2004, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <QtGui/QDialog>

#include "exports.h"

class QBoxLayout;
class QLabel;

/**
	Odpowiednik klasy QMessageBox z QT. W zamierzeniu ma
	mie� wi�ksze mo�liwo�ci i wygodniejszy interface ni�
	orygina�.
**/
class KADUAPI MessageDialog : public QDialog
{
	Q_OBJECT

	static QMap<QString, MessageDialog *> Boxes;
	QLabel *icon;
	QString message;

	void addButton(QBoxLayout *parent, const QString &caption, const char *slot);

private slots:
	void okClicked();
	void cancelClicked();
	void yesClicked();
	void noClicked();

protected:
	void closeEvent(QCloseEvent *e);

public:
	static const int OK;
	static const int CANCEL;
	static const int YES;
	static const int NO;
	static const int RETRY;

	MessageDialog(const QString &message, int components = 0, bool modal = false, const QString &iconPath = QString::null, QWidget *parent = 0);
	~MessageDialog();


	/**
		Informuje u�ytkownika o wykonywanej przez
		program czynno�ci. Tworzy dialog zawieraj�cy
		tylko podany tekst. Nie blokuje wykonywania
		programu. Zamkni�cie dialogu nast�puje po
		wywo�aniu funkcji close z t� sam� wiadomo�ci�
		przekazan� jako argument.
		Wywo�uje QApplication::processEvents().
	**/
	static void status(const QString &message);

	/**
		Zadaje u�ytkownikowi pytanie. Tworzy dialog
		z dwoma przyciskami: "Tak" i "Nie". Blokuje
		wykonywanie programu i czeka na reakcj�
		u�ytkownika. Zwraca true je�li wybra� "Tak"
		lub false je�li wybra� "Nie".
	**/
	static bool ask(const QString &message, const QString &iconPath = QString::null, QWidget *parent = 0);

	/**
		Przekazuje u�ytkownikowi informacj�. Tworzy
		dialog z przyciskiem: "OK" zamykaj�cy okno.
		Nie blokuje wykonywania programu.
	**/
	static void msg(const QString &message, bool modal = false, const QString &iconPath = QString::null, QWidget *parent = 0);

	/**
		Zamyka dialog, kt�ry zosta� stworzony za
		pomoc� funkcji status z t� sam� wiadomo�ci�
		przekazan� jako argument.
	**/
	static void close(const QString &message);

signals:
	void okPressed();
	void cancelPressed();
	void yesPressed();
	void noPressed();

};

#endif // MESSAGE_BOX_H
