/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_CHANGER
#define STATUS_CHANGER

#include <QtCore/QObject>

#include "protocols/status.h"

#include "exports.h"

class QTimer;

/**
	@class StatusChanger
	@brief Podstawowa klasa s�u��ca do zmiany statusu u�ytkownika.

	Implementacje klasy dokonuj� zmiany statusu u�ytkownik�w na podstawie dowolnych, zale�nych od
	klasy parametr�w. Zmianie mo�e ulega� zar�wno opis jak i stan dost�pno�ci.
 **/
class KADUAPI StatusChanger : public QObject
{
	Q_OBJECT

	int Priority;

public:
	/**
		Podstawowy kontruktor. Tworzy modyfikator statusu z zadanym priorytetem.
		Modyfikatory aplikowane s� w kolejno�ci rosn�cych priorytet�w.

		@arg priority priorytet modyfikatora
	 **/
	StatusChanger(int priority);
	virtual ~StatusChanger();

	int priority();

	/**
		Metoda zmieniaj�ca status.

		@arg status aktualny status u�ytkownika, metoda mo�e zmieni� warto�� argumentu
	 **/
	virtual void changeStatus(Status &status) = 0;

signals:
	void statusChanged();

};

/**
	@class UserStatusChanger
	@brief Klasa umo�liwiaj�ca u�ytkownikowi ustawienie w�asnego statusu.

	Klasa posiada pole userStatus okre�laj�ce status, jaki u�ytkownik sam sobie ustawi�.
	Klasa modyfikuje aktualny status u�ytkownika ustawiaj�c go na warto�� pola userStatus.
	Priorytet modyfikatora: 0.
 **/
class KADUAPI UserStatusChanger : public StatusChanger
{
	Q_OBJECT

	Status UserStatus;

public:
	UserStatusChanger();
	virtual ~UserStatusChanger();

	virtual void changeStatus(Status &status);
	const Status & status() { return UserStatus; }

public slots:
	/**
		Zapisuje status, jaki u�ytkownik sobie ustawi�.

		@param status status u�ytkownika
	 **/
	void userStatusSet(const Status &status);

};

/**
	@class SplitStatusChanger
	@brief Klasa dziel�ca d�ugie opisy na kr�tsze.

	Klasa dzieli d�ugi opis na kilka kr�tszych, kt�re wy�wietlane s� w odst�pie 30 sekund od siebie.
	Dla przyk�adu, opis "ten opis ma wi�cej ni� dwadzie�cia znak�w" przy maksymalnej d�ugo�ci opisu
	wynosz�cej dwadzie�cia znak�w zostanie podzielony na nast�puj�ce pod-opisy:

		"ten opis ma wi� --->"
		"<--- cej ni� dw --->"
		"<--- adzie�cia  --->"
		"<--- znak�w"

	W Gadu-Gadu limit d�ugo�ci wynosi 70 znak�w.

	Priorytet: 1000.
 **/
class KADUAPI SplitStatusChanger : public StatusChanger
{
	Q_OBJECT

	int splitSize;

	int descriptionSplitBegin;
	int descriptionSplitLength;

	QTimer *splitTimer;
	QString lastDescription;

	bool enabled;

private slots:
	void timerInvoked();

public:
	/**
		Tworzy nowy obiekt dziel�cy opisy na pod-opisy o maksymalnej d�ugo�ci splitSize znak�w.

		@arg splitSize maksymalna d�ugo�� pod-opisu
	 **/
	SplitStatusChanger(unsigned int splitSize);
	virtual ~SplitStatusChanger();

	virtual void changeStatus(Status &status);

	void enable();
	void disable();

};

/**
	@class StatusChangerManager
	@brief Manager modyfikator�w statusu.

	Klasa zarz�dza modyfikatorami statusu. Do��cza i od��cza na ��danie nowy modyfikatory.
	Wywo�uje poszczeg�lne modyfikatory w kolejno�ci ich rosn�cych priorytet�w.

	Po stworzeniu i dodaniu pierwszych modyfikator�w wymaga uruchomienia za pomoc� metody enable.
 **/
class KADUAPI StatusChangerManager : public QObject
{
	Q_OBJECT

	QList<StatusChanger *> statusChangers;
	Status LastStatus;
	bool enabled;

public:
	static void initModule();
	static void closeModule();

	StatusChangerManager();
	virtual ~StatusChangerManager();

	/**
		Aktywuje modyfikatory.
	 **/
	void enable();

	void registerStatusChanger(StatusChanger *statusChanger);
	void unregisterStatusChanger(StatusChanger *statusChanger);

	Status status() { return LastStatus; }

public slots:
	/**
		Slot wywo�ywany, gdy jeden z modyfikator�w dokona zmiany opisu.
		Wywo�anie powoduje ponowne przetworzenie statusu przez wszystkie modyfikatory
		i wys�anie sygna�u statusChanged(UserStatus).
	 **/
	void statusChanged();

signals:
	/**
		Sygna� wysy�any, gdy zostanie ustalony ostateczny status u�ytkownika.
	 **/
	void statusChanged(Status);

};

extern KADUAPI StatusChangerManager *status_changer_manager;

#endif // STATUS_CHANGER
