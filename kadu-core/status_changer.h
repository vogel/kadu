#ifndef STATUS_CHANGER
#define STATUS_CHANGER

#include <qglobal.h>

#include <qobject.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "status.h"

class QTimer;

/**
	@class StatusChanger
	@brief Podstawowa klasa s³u¿±ca do zmiany statusu u¿ytkownika.

	Implementacje klasy dokonuj± zmiany statusu u¿ytkowników na podstawie dowolnych, zale¿nych od
	klasy parametrów. Zmianie mo¿e ulegaæ zarówno opis jak i stan dostêpno¶ci.
 **/
class StatusChanger : public QObject
{
	Q_OBJECT

	int Priority;

public:
	/**
		Podstawowy kontruktor. Tworzy modyfikator statusu z zadanym priorytetem.
		Modyfikatory aplikowane s± w kolejno¶ci rosn±cych priorytetów.

		@arg priority priorytet modyfikatora
	 **/
	StatusChanger(int priority);
	virtual ~StatusChanger();

	int priority();

	/**
		Metoda zmieniaj±ca status.

		@arg status aktualny status u¿ytkownika, metoda mo¿e zmieniæ warto¶æ argumentu
	 **/
	virtual void changeStatus(UserStatus &status) = 0;

signals:
	void statusChanged();

};

/**
	@class UserStatusChanger
	@brief Klasa umo¿liwiaj±ca u¿ytkownikowi ustawienie w³asnego statusu.

	Klasa posiada pole userStatus okre¶laj±ce status, jaki u¿ytkownik sam sobie ustawi³.
	Klasa modyfikuje aktualny status u¿ytkownika ustawiaj±c go na warto¶æ pola userStatus.
	Priorytet modyfikatora: 0.
 **/
class UserStatusChanger : public StatusChanger
{
	Q_OBJECT

	UserStatus userStatus;

public:
	UserStatusChanger();
	virtual ~UserStatusChanger();

	virtual void changeStatus(UserStatus &status);
	const UserStatus & status() { return userStatus; }

public slots:
	/**
		Zapisuje status, jaki u¿ytkownik sobie ustawi³.

		@param status status u¿ytkownika
	 **/
	void userStatusSet(UserStatus &status);

};

/**
	@class SplitStatusChanger
	@brief Klasa dziel±ca d³ugie opisy na krótsze.

	Klasa dzieli d³ugi opis na kilka krótszych, które wy¶wietlane s± w odstêpie 30 sekund od siebie.
	Dla przyk³adu, opis "ten opis ma wiêcej ni¿ dwadzie¶cia znaków" przy maksymalnej d³ugo¶ci opisu
	wynosz±cej dwadzie¶cia znaków zostanie podzielony na nastêpuj±ce pod-opisy:

		"ten opis ma wiê --->"
		"<--- cej ni¿ dw --->"
		"<--- adzie¶cia  --->"
		"<--- znaków"

	W Gadu-Gadu limit d³ugo¶ci wynosi 70 znaków.

	Priorytet: 1000.
 **/
class SplitStatusChanger : public StatusChanger
{
	Q_OBJECT

	unsigned int splitSize;

	unsigned int descriptionSplitBegin;
	unsigned int descriptionSplitLength;

	QTimer *splitTimer;
	QString lastDescription;

	bool enabled;

private slots:
	void timerInvoked();

public:
	/**
		Tworzy nowy obiekt dziel±cy opisy na pod-opisy o maksymalnej d³ugo¶ci splitSize znaków.

		@arg splitSize maksymalna d³ugo¶æ pod-opisu
	 **/
	SplitStatusChanger(unsigned int splitSize);
	virtual ~SplitStatusChanger();

	virtual void changeStatus(UserStatus &status);

	void enable();
	void disable();

};

/**
	@class StatusChangerManager
	@brief Manager modyfikatorów statusu.

	Klasa zarz±dza modyfikatorami statusu. Do³±cza i od³±cza na ¿±danie nowy modyfikatory.
	Wywo³uje poszczególne modyfikatory w kolejno¶ci ich rosn±cych priorytetów.

	Po stworzeniu i dodaniu pierwszych modyfikatorów wymaga uruchomienia za pomoc± metody enable.
 **/
class StatusChangerManager : public QObject
{
	Q_OBJECT

	Q3ValueList<StatusChanger *> statusChangers;
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

public slots:
	/**
		Slot wywo³ywany, gdy jeden z modyfikatorów dokona zmiany opisu.
		Wywo³anie powoduje ponowne przetworzenie statusu przez wszystkie modyfikatory
		i wys³anie sygna³u statusChanged(UserStatus).
	 **/
	void statusChanged();

signals:
	/**
		Sygna³ wysy³any, gdy zostanie ustalony ostateczny status u¿ytkownika.
	 **/
	void statusChanged(UserStatus);

};

extern StatusChangerManager *status_changer_manager;

#endif // STATUS_CHANGER
