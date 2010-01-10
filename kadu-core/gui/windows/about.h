/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ABOUT_H
#define ABOUT_H

#include <QtGui/QWidget>

/**
	\class About
	\brief Okno z informacjami o programie.
**/
class About : public QWidget 
{
	Q_OBJECT

	QString loadFile(const QString &name);
	void keyPressEvent(QKeyEvent *);

public:
	/**
		\fn About(QWidget *parent = 0)
		Konstruktor tworzy okno, ustawia zak�adki z informacjami o autorach, podzi�kowaniach, licencji oraz list� zmian w programie. \n
		Domy�lnie okno ma rozmiar 640x420.
		\param parent rodzic okna
		\param name nazwa obiektu
	**/
	About(QWidget *parent = 0);

	/**
		\fn ~About()
		Destruktor zapisuje rozmiar okna.
	**/
	~About();

};

#endif
