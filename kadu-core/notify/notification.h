/*
 * %kadu copyright begin%
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QtCore/QPair>
#include <QtGui/QIcon>

#include "buddies/buddy-set.h"
#include "exports.h"

class QTimer;

/**
	@class Notification
	@author Rafa� 'Vogel' Malinowski
	@brief Klasa informuj�ca u�ytkownika o zaj�ciu pewnego zdarzenia.

	Zdarzenia identyfikowane s� poprzez nazwy. Ka�de zdarzenie musi zosta� zarejestrowane
	przed u�yciem w klasie Notify a takze wyrejestrowane w przypadku wy�adowania modu�u,
	kt�ry wywo�ywa� dane zdarzenie.

	Do ka�dego zdarzenia mo�e by� przypisana ikona oraz lista akcji, kt�re u�ytkownik
	mo�e podj�� w odpowiedzi na zdarzenie. Przyk�adowo, zdarzenie polegaj�ce na odebraniu wiadomo�ci od
	anonimowego kontaktu, z kt�rym nie prowadzimy aktualnie rozmowy mo�e mie� posta�:

	<ul>
		<li>nazwa: newChatWithAnonymous</li>
		<li>ikona: chat</li>
		<li>akcja: odbierz komunikat</li>
		<li>akcja: ignoruj komunikat</li>
	</ul>

	Akcje u�ytkownika implementowane s� w obiektach potomnych w postaci w postaci slot�w.
	Domy�lnie zaimplementowane s� dwa sloty wirtualne: callbackAccept i callbackDiscard,
	odpowiadaj�ce zaakceptowaniu i odrzuceniu zdarzenia.

	Dodanie nowych akcji polega na dodaniu nowych slot�w do klas potomnych. Aktywacja
	akcji (tak�e tych dw�ch domy�lnych) dokonuje sie poprzez metod� addCallback(caption, slot),
	na przyk�ad:

	<code>
		addCallback(tr("Odbierz rozmow�"), SLOT(callbackAccept()));
		addCallback(tr("Ignoruj rozmow�"), SLOT(callbackDiscard()));
	</code>

	Obiekty mog� automatycznie podejmowa� akcje domy�lne po przekroczeniu pewnego
	okresu czasu:

	<code>
		setDefaultCallback(2000, SLOT(callbackAccept()));
	</code>

	Mo�na takze anulowa� akcj� domy�ln�:

	<code>
		clearDefaultCallback();
	</code>

	Notyfikacja, co do kt�rej pewna akcja zosta�a wykonana, wywo�uje sygna� closed().
 **/
class KADUAPI Notification : public QObject
{
	Q_OBJECT

public:
	struct Callback
	{
		QString Caption;
		const char *Slot;
		const char *Signature;

		Callback() : Slot(0), Signature(0) {}

		Callback(const Callback &copy)
		{
			Caption = copy.Caption;
			Slot = copy.Slot;
			Signature = copy.Signature;
		}

		Callback & operator = (const Callback &copy)
		{
			Caption = copy.Caption;
			Slot = copy.Slot;
			Signature = copy.Signature;

			return *this;
		}
	};

private:

	QString Type;

	QString Title;
	QString Text;
	QString Details;
	QString IconPath;
	QIcon Icon;

	QList<Callback> Callbacks;
	QTimer *DefaultCallbackTimer;

	int ReferencesCount;
	bool Closing;

public:

	/**
		Tworzy now� notfikacj� o zadanym typie, ikonie i zadanej asocjacji z kontaktami.
		Klasy potome zazwyczaj maj� w�asne kontruktory, kt�re wype�niaj� automatycznie
		parametry tego konstruktora.

		@arg type typ zdarzenia
		@arg icon nazwa ikony zdarzenia
	 **/
	Notification(const QString &type, const QString &iconPath);
	virtual ~Notification();

	/**
		Wywo�ywane przez notyfikator, kt�ry zajmuje si� danym zdarzeniem.
	 **/
	void acquire();
	/**
		Wywo�ywane przez notyfikator, kt�ry przestaje zajmowa� si� danym zdarzeniem.
		Gdy �aden notyfikator nie zajmuje si� danym zdarzeniem, zdarzenie jest zwalniane.
		Wyst�puje to na przyk�ad w przypadku modu��w d�wi�kowych czy modu�u hints, gdy
		dymek zniknie po up�ywie okre�lonego czasu a nie przez zdarzenie wywo�ane przez u�ytkownika.
	 **/
	void release();

	/**
		Zamyka zdarzenie. Wywo�uje sygna� closed() i usuwa obiekt.
	 **/
	void close();

	/**
		Usuwa akcje u�ytkownika
	 **/
	void clearCallbacks();
	/**
		Dodaje akcje u�ytkownika.

		@arg caption wy�wietlana nazwa akcji
		@arg slot slot wywo�ywany w przypadku wybrania przez u�ytkownika danej akcji
	 **/
	void addCallback(const QString &caption, const char *slot, const char *signature);

	/**
		Ustawia akcj� domy�ln�.

		@arg timeout liczba milisekund, jaka musi up�yn�� przed wykonaniem domy�lnej akcji
		@arg slot slot wywo�ywany w przypadku up�yni�cia czasu
	 **/
	void setDefaultCallback(int timeout, const char *slot);

	virtual bool requireCallback() { return false; }
	virtual bool ignoreSilentMode() { return false; }

	/**
		Typ zdarzenia.

		@return typ zdarzenia
	 **/
	QString type() const;

	QString key() const;

	/**
		Ustawia tytu� zdarzenia.
	 **/
	void setTitle(const QString &title);
	/**
		Tytu� zdarzenia.

		@return tytu� zdarzenia
	 **/
	QString title() const;

	/**
		Ustawia tre�� zdarzenia.
	 **/
	void setText(const QString &text);
	/**
		Tre�� zdarzenia.

		@return tre�� zdarzenia
	 **/
	QString text() const;

	/**
		Ustawia szczeg��y zdarzenia (jak na przyk�ad tekst wiadomo�ci).
	 **/
	void setDetails(const QString &details);
	/**
		Szczeg��y zdarzenia
	 **/
	QString details() const;

	/**
		Ustawia ikon� zdarzenia.
	 **/
	void setIcon(const QString &iconPath);
	/**
		Ikona zdarzenia.

		@return ikona zdarzenia
	 **/
	QIcon icon() const;

	QString iconPath() const;
	
	/**
		Lista akcji.

		@return lista akcji
	 **/
	const QList<Callback> & getCallbacks();

public slots:
	/**
		Domy�lny slot wywo�ywany w przypadku zaakceptowania zdarzenia.
	 **/
	virtual void callbackAccept();
	/**
		Domy�lny slot wywo�ywany w przypadku odrzucenia zdarzenia.
	 **/
	virtual void callbackDiscard();

	/**
		Slot anuluj�cy domy�ln� akcj� - wywo�ywany r�cznie przy wyborze dowolnej innej akcji.
	 **/
	void clearDefaultCallback();

signals:
	/**
		Sygna� wysylany przy zamykaniu zdarzenia, po wyborze przez u�ytkownika dowolnej akcji.
	 **/
	void closed(Notification *);

};

#endif // NOTIFICATION_H
