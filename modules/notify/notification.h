/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <qmap.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include "userlist.h"

#include "../notify/notification.h"

class QTimer;

/**
	@class Notification
	@author Rafa³ 'Vogel' Malinowski
	@brief Klasa informuj±ca u¿ytkownika o zaj¶ciu pewnego zdarzenia.

	Zdarzenia identyfikowane s± poprzez nazwy. Ka¿de zdarzenie musi zostaæ zarejestrowane
	przed u¿yciem w klasie Notify a takze wyrejestrowane w przypadku wy³adowania modu³u,
	który wywo³ywa³ dane zdarzenie.

	Do ka¿dego zdarzenia mo¿e byæ przypisana ikona, lista kontaktów oraz lista akcji, które u¿ytkownik
	mo¿e podj±æ w odpowiedzi na zdarzenie. Przyk³adowo, zdarzenie polegaj±ce na odebraniu wiadomo¶ci od
	anonimowego kontaktu, z którym nie prowadzimy aktualnie rozmowy mo¿e mieæ postaæ:

	<ul>
		<li>nazwa: newChatWithAnonymous</li>
		<li>ikona: chat</li>
		<li>lista kontaktów: kontakt, który rozpocz±³ rozmowê</li>
		<li>akcja: odbierz komunikat</li>
		<li>akcja: ignoruj komunikat</li>
	</ul>

	Akcje u¿ytkownika implementowane s± w obiektach potomnych w postaci w postaci slotów.
	Domy¶lnie zaimplementowane s± dwa sloty wirtualne: callbackAccept i callbackDiscard,
	odpowiadaj±ce zaakceptowaniu i odrzuceniu zdarzenia.

	Dodanie nowych akcji polega na dodaniu nowych slotów do klas potomnych. Aktywacja
	akcji (tak¿e tych dwóch domy¶lnych) dokonuje sie poprzez metodê addCallback(caption, slot),
	na przyk³ad:

	<code>
		addCallback(tr("Odbierz rozmowê"), SLOT(callbackAccept()));
		addCallback(tr("Ignoruj rozmowê"), SLOT(callbackDiscard()));
	</code>

	Obiekty mog± automatycznie podejmowaæ akcje domy¶lne po przekroczeniu pewnego
	okresu czasu:

	<code>
		setDefaultCallback(2000, SLOT(callbackAccept()));
	</code>

	Mo¿na takze anulowaæ akcjê domy¶ln±:

	<code>
		clearDefaultCallback();
	</code>

	Notyfikacja, co do której pewna akcja zosta³a wykonana, wywo³uje sygna³ closed().
 **/
class Notification : public QObject {
	Q_OBJECT

private:

	QString Type;
	UserListElements Ule;

	QString Title;
	QString Text;
	QString Details;
	QString Icon;

	QValueList<QPair<QString, const char *> > Callbacks;
	QTimer *DefaultCallbackTimer;

	int ReferencesCount;
	bool Closing;

public:

	/**
		Tworzy now± notfikacjê o zadanym typie, ikonie i zadanej asocjacji z kontaktami.
		Klasy potome zazwyczaj maj± w³asne kontruktory, które wype³niaj± automatycznie
		parametry tego konstruktora.

		@arg type typ zdarzenia
		@arg icon nazwa ikony zdarzenia
		@arg userListElements lista kontaktów powi±zana ze zdarzeniem
	 **/
	Notification(const QString &type, const QString &icon, const UserListElements &userListElements);
	virtual ~Notification();

	/**
		Wywo³ywane przez notyfikator, który zajmuje siê danym zdarzeniem.
	 **/
	void acquire();
	/**
		Wywo³ywane przez notyfikator, który przestaje zajmowaæ siê danym zdarzeniem.
		Gdy ¿aden notyfikator nie zajmuje siê danym zdarzeniem, zdarzenie jest zwalniane.
		Wystêpuje to na przyk³ad w przypadku modu³ów d¼wiêkowych czy modu³u hints, gdy
		dymek zniknie po up³ywie okre¶lonego czasu a nie przez zdarzenie wywo³ane przez u¿ytkownika.
	 **/
	void release();

	/**
		Zamyka zdarzenie. Wywo³uje sygna³ closed() i usuwa obiekt.
	 **/
	void close();

	/**
		Usuwa akcje u¿ytkownika
	 **/
	void clearCallbacks();
	/**
		Dodaje akcje u¿ytkownika.

		@arg caption wy¶wietlana nazwa akcji
		@arg slot slot wywo³ywany w przypadku wybrania przez u¿ytkownika danej akcji
	 **/
	void addCallback(const QString &caption, const char *slot);

	/**
		Ustawia akcjê domy¶ln±.

		@arg timeout liczba milisekund, jaka musi up³yn±æ przed wykonaniem domy¶lnej akcji
		@arg slot slot wywo³ywany w przypadku up³yniêcia czasu
	 **/
	void setDefaultCallback(int timeout, const char *slot);

	/**
		Typ zdarzenia.

		@return typ zdarzenia
	 **/
	QString type() const;
	/**
		Lista kontaktów powi±zana ze zdarzeniem/

		@return lista kontaktów powi±zana ze zdarzeniem
	 **/
	const UserListElements &userListElements() const;

	/**
		Ustawia tytu³ zdarzenia.
	 **/
	void setTitle(const QString &title);
	/**
		Tytu³ zdarzenia.

		@return tytu³ zdarzenia
	 **/
	QString title() const;

	/**
		Ustawia tre¶æ zdarzenia.
	 **/
	void setText(const QString &text);
	/**
		Tre¶æ zdarzenia.

		@return tre¶æ zdarzenia
	 **/
	QString text() const;

	/**
		Ustawia szczegó³y zdarzenia (jak na przyk³ad tekst wiadomo¶ci).
	 **/
	void setDetails(const QString &details);
	/**
		Szczegó³y zdarzenia
	 **/
	QString details() const;

	/**
		Ustawia ikonê zdarzenia.
	 **/
	void setIcon(const QString &icon);
	/**
		Ikona zdarzenia.

		@return ikona zdarzenia
	 **/
	QString icon() const;

	/**
		Lista akcji.

		@return lista akcji
	 **/
	const QValueList<QPair<QString, const char *> > & getCallbacks();

public slots:
	/**
		Domy¶lny slot wywo³ywany w przypadku zaakceptowania zdarzenia.
	 **/
	virtual void callbackAccept();
	/**
		Domy¶lny slot wywo³ywany w przypadku odrzucenia zdarzenia.
	 **/
	virtual void callbackDiscard();

	/**
		Slot anuluj±cy domy¶ln± akcjê - wywo³ywany rêcznie przy wyborze dowolnej innej akcji.
	 **/
	void clearDefaultCallback();

signals:
	/**
		Sygna³ wysylany przy zamykaniu zdarzenia, po wyborze przez u¿ytkownika dowolnej akcji.
	 **/
	void closed(Notification *);

};

#endif // NOTIFICATION_H
