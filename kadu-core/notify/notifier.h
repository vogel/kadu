/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QtCore/QObject>

#include "icons/kadu-icon.h"
#include "exports.h"

class Notification;
class NotifierConfigurationWidget;

/**
	@class Notifier
	@brief Klasa abstrakcyjna opisuj�ca notifikator.

	Notifykatory zajmuj� si� wy�wietlaniem lub informowaniem w inny spos�b u�ytkownika o wyst�puj�cych
	w programie zdarzeniach (nowa rozmowa, nowy transfer pliku, b��d...).

	Notyfikatory mog� umo�liwia� u�ytkownikowi podj�cie akcji jak odebranie lub zignorownie rozmowy,
	odebranie pliku, kontynuacje odbierania pliku i inne. Niekt�ry notifikatory nie b�d�
	implementowa� akcji, dlatego te� niekt�re zdarzenia nie mog� by� przez nie obs�ugiwane.
 **/
class KADUAPI Notifier : public QObject
{
	Q_OBJECT

public:
	/**
		@enum CallbackCapacity
		Okre�la, czy dany notifikator potrafi obs�u�y� zdarzenia wymagajace od u�ytkownika akcji innej ni� domy�lne zaakceptowanie/odrzucenie.
	 **/
	enum CallbackCapacity {
		CallbackSupported,
		CallbackNotSupported
	};

private:
	QString Name;
	QString Description;
	KaduIcon Icon;

public:
	Notifier(const QString &name, const QString &description, const KaduIcon &icon, QObject *parent = 0);
	virtual ~Notifier();

	const QString & name() const { return Name; }
	const QString & description() const { return Description; }
	const KaduIcon & icon() const { return Icon; }

	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

	/**
		Metoda informuj�ca notifikator o nowym zdarzeniu. Zdarzenie mo�e wywo�a�
		sygna� closed(), po kt�rym notyfikator musi przesta� informowa� u�ytkownika
		o danym zdarzeniu (na przyk�ad, musi zamkn�� skojarzone ze zdarzeniem okno).
	 **/
	virtual void notify(Notification *notification) = 0;

	/**
		Zwraca widget, jaki zostanie dodany do okna konfiguracyjnego
		na prawo od odpowiedniego CheckBoxa.
		Mo�e zwr�ci� zero.
	 **/
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0) = 0;

};

#endif // NOTIFEIR_H
