/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QtCore/QObject>
#include <QtGui/QIcon>

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
class KADUAPI Notifier : public virtual QObject
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
	QIcon Icon;


public:
	explicit Notifier(const QString name,  QIcon icon, QObject *parent = 0);
	virtual ~Notifier();

	QString name() { return Name; }
	QIcon & icon() { return Icon; }


	virtual CallbackCapacity callbackCapacity();

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
