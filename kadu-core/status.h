#ifndef KADU_STATUS_H
#define KADU_STATUS_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "exports.h"

class QPixmap;

/**
	\enum eUserStatus
	Opisuje stan statusu.
**/
enum eUserStatus
{
	Online, /*!< Dost�pny */
	Busy, /*!< Zaraz wracam */
	Invisible, /*!< Ukryty */
	Offline, /*!< Niedost�pny */
	FFC, /*!< Porozmawiaj ze mna */
	DND, /*!< Nie przeszkadzac */
	Blocking /*!< Blokuj�cy */
};

/**
	Klasa ta zawiera informacje o statusie kontaktu lub u�ytkownika Kadu, oraz metody
	s�u��ce do zmiany tego statusu.
	\class UserStatus
	\brief Klasa opisuj�ca status.
**/
class KADUAPI UserStatus : public QObject
{
	Q_OBJECT

	bool Changed;

protected:
	/**
		\var eUserStatus Stat
		Zawiera informacj� o aktualnym statusie w postaci enumerycznej.
	**/
	eUserStatus Stat;

	/**
		\var QString Description
		Zawiera aktualny opis.
	**/
	QString Description;

	/**
		\var bool FriendsOnly
		Zawiera informacj� o trybie "tylko dla znajomych".
	**/
	bool FriendsOnly;

public:
	/**
		\fn UserStatus()
		Standardowy konstruktor.
	**/
	UserStatus();
	virtual ~UserStatus();

	/**
		\fn UserStatus(const UserStatus &copyMe)
		Konstruktor kopiuj�cy.
		\param copyMe obiekt kt�ry chcemy skopiowa�.
	**/
	UserStatus(const UserStatus &copyMe);

	/**
		\fn void operator = (const UserStatus &copyMe)
		Operator kopiuj�cy.
		\param copyMe obiekt kt�ry chcemy skopiowa�.
	**/
	UserStatus &operator = (const UserStatus &copyMe);

	/**
		\fn bool operator == (const UserStatus &compare) const
		Operator por�wnuj�cy. Sprawdza to�samo�� obiekt�w.
		\param compare obiekt z kt�rym chcemy por�wna� bierz�cy obiekt.
	**/
	bool operator == (const UserStatus &compare) const;

	/**
		\fn bool operator != (const UserStatus &compare) const
		Operator por�wnuj�cy. Sprawdza r��nic� pomi�dzy obiektami.
		\param compare obiekt z kt�rym chcemy por�wna� bierz�cy obiekt.
	**/
	bool operator != (const UserStatus &compare) const;

	/**
		\fn virtual UserStatus *copy() const
		Wykonuje kopi� obiektu, kt�r� p��niej nale�y samodzielnie zniszczy�
	**/
	virtual UserStatus *copy() const;

	/**
		\fn bool isOnline() const
		Sprawdza czy kontakt jest "dost�pny".
		\return TRUE je�li jest "dost�pny", FALSE w przeciwnym wypadku.
	**/
	bool isOnline() const;

	/**
		\fn bool isBusy() const
		Sprawdza czy kontakt ma status "zaraz wracam".
		\return TRUE je�li ma status "zaraz wracam", FALSE w przeciwnym wypadku.
	**/
	bool isBusy() const;

	/**
		\fn bool isInvisible() const
		Sprawdza czy kontakt jest "ukryty".
		\return TRUE je�li jest "ukryty", FALSE w przeciwnym wypadku.
	**/
	bool isInvisible() const;

	/**
		\fn bool isOffline() const
		Sprawdza czy kontakt jest "niedost�pny".
		\return TRUE je�li jest "niedost�pny", FALSE w przeciwnym wypadku.
	**/
	bool isOffline() const;

	/**
		\fn bool isBlocking() const
		Sprawdza czy kontakt jest "blokuj�cy".
		\return TRUE je�li jest "blokuj�cy", FALSE w przeciwnym wypadku.
	**/
	bool isBlocking() const;

	/**
			\fn static bool isOffline(int index)
		Sprawdza czy status dany w postaci indeksowej jest statusem "niedost�pny" lub jego
		odpowiednikiem z opisem.
		\return TRUE je�li status oznacza "niedost�pny", FALSE w przeciwnym wypadku.
	**/
	static bool isOffline(int index);

	/**
		\fn bool isTalkWithMe() const
		Sprawdza czy kontakt jest "porozmawiaj ze mna".
		\return TRUE je�li jest "porozmawiaj ze mna", FALSE w przeciwnym wypadku.
	**/
	bool isTalkWithMe() const;

	/**
		\fn bool isDnoNotDisturb() const
		Sprawdza czy kontakt jest "nie przeszkadzac".
		\return TRUE je�li jest "nie przeszkadzac", FALSE w przeciwnym wypadku.
	**/
	bool isDoNotDisturb() const;

	/**
		\fn bool isAvailable() const
		\return TRUE je�li status jest Online, Busy, TalkWithMe lub DoNotDisturb
	**/
	bool isAvailable() const;

	/**
		\fn bool hasDescription() const
		Sprawdza czy status jest z opisem.
		\return TRUE gdy status jest z opisem, FALSE w przeciwnym wypadku.
	**/
	bool hasDescription() const;

	/**
		\fn bool isFriendsOnly() const
		Sprawdza czy status jest "tylko dla znajomych".
		\return TRUE je�li jest "tylko dla znajomych", FALSE w przeciwnym wypadku.
	**/
	bool isFriendsOnly() const;

	/**
		\fn QString description() const
		\return opis, o ile status jest z opisem, lub "" gdy opisu nie ma.
	**/
	QString description() const;

	/**
		\fn eUserStatus status() const
		\return status w postaci enumeracyjnej.
	**/
	eUserStatus status() const;

	/**
		\fn int index() const
		\return status w postaci indeksowej.
	**/
	int index() const;

	/**
		\fn static int index(eUserStatus stat, bool has_desc)
		Zamienia enumeracyjn� posta� statusu na indeksow�.
		\param stat enumeracyjna posta� statusu.
		\param has_desc informuje, czy jest to odmiana statusu z opisem.
		\return status w postaci indeksowej.
	**/
	static int index(eUserStatus stat, bool has_desc);

	/**
		\fn virtual QPixmap pixmap(bool mobile = false) const
		Zwraca ikon� reprezentuj�c� ten status.
		\param mobile informuje czy ikona ma zawiera� element telefonu kom�rkowego. Domy�lnie FALSE.
		\return pixmap� reprezentuj�c� ikon�.
	**/
	virtual QPixmap pixmap(bool mobile = false) const;

	/**
		\fn virtual QString pixmap(bool mobile = false) const
		Zwraca nazw� ikonki reprezentuj�cej ten status.
		\param mobile informuje czy ikona ma zawiera� element telefonu kom�rkowego. Domy�lnie FALSE.
		\return nazwa pixmapy reprezentuj�cej ikon�.
	**/
	virtual QString pixmapName(bool mobile = false) const;

	/**
		\fn virtual QPixmap pixmap(const UserStatus &stat, bool mobile = false) const
		Zwraca ikon� reprezentuj�c� dany status.
		\param stat status kt�rego ikon� nale�y zwr�ci�
		\param mobile informuje czy ikona ma zawiera� element telefonu kom�rkowego. Domy�lnie FALSE.
		\return pixmap� reprezentuj�c� ikon�.
	**/
	virtual QPixmap pixmap(const UserStatus &, bool mobile = false) const;

	/**
		\fn virtual QString pixmap(bool mobile = false) const
		Zwraca nazw� ikonki reprezentuj�cej dany status.
		\param stat status kt�rego ikon� nale�y zwr�ci�
		\param mobile informuje czy ikona ma zawiera� element telefonu kom�rkowego. Domy�lnie FALSE.
		\return nazwa pixmapy reprezentuj�cej ikon�.
	**/
	virtual QString pixmapName(const UserStatus &, bool mobile = false) const;

	/**
		\fn virtual QPixmap pixmap(eUserStatus stat, bool has_desc, bool mobile = false) const
		Zwraca ikon� reprezentuj�c� dany status.
		\param stat status w postaci enumeracyjnej kt�rego ikon� nale�y zwr�ci�
		\param has_desc informuje, czy jest to odmiana statusu z opisem.
		\param mobile informuje czy ikona ma zawiera� element telefonu kom�rkowego. Domy�lnie FALSE.
		\return pixmap� reprezentuj�c� ikon�.
	**/
	virtual QPixmap pixmap(eUserStatus stat, bool has_desc, bool mobile = false) const;

	/**
		\fn virtual QString pixmap(bool mobile = false) const
		Zwraca nazw� ikonki reprezentuj�c� dany status.
		\param stat status w postaci enumeracyjnej kt�rego ikon� nale�y zwr�ci�
		\param has_desc informuje, czy jest to odmiana statusu z opisem.
		\param mobile informuje czy ikona ma zawiera� element telefonu kom�rkowego. Domy�lnie FALSE.
		\return nazwa pixmapy reprezentuj�cej ikon�.
	**/
	virtual QString pixmapName(eUserStatus stat, bool has_desc, bool mobile = false) const;

	/**
		\fn static eUserStatus fromString(const QString& stat)
		Zamienia status z formy �a�cucha znak�w na posta� enumeracyjn�.
		\param stat �a�cuch znak�w reprezentuj�cy nazw� statusu. Poprawne nazwy status�w to:
			\arg \c Online
			\arg \c Busy
			\arg \c Invisible
			\arg \c Blocking
		\return enumeracyjna posta� statusu.
	**/
	static eUserStatus fromString(const QString& stat);

	/**
		\fn static QString toString(eUserStatus stat, bool has_desc)
		Zamienia status z postaci enumeracyjnej na form� �a�cucha znak�w.
		\param stat posta� enumeracyjna statusu
		\param has_desc informuje, czy jest to odmiana statusu z opisem.
		\return enumeracyjna posta� statusu.
	**/
	static QString toString(eUserStatus stat, bool has_desc);

	/**
		\fn QString toString(eUserStatus stat, bool has_desc)
		Zamienia status na form� �a�cucha znak�w.
		robi dok�adnie tyle co UserStatus::toString(this->status(), this->hasDescription())
		\see toString(eUserStatus, bool)
		\return status w postaci napisu.
	**/
	QString toString() const;

	/**
		\fn static int count()
		\return liczb� wszystkich mo�liwych status�w, ��cznie z "blokowany" oraz "niedost�pny z opisem".
	**/
	static int count();

	/**
		\fn static int count()
		\return liczb� wszystkich mo�liwych status�w, kt�re mo�e zobaczy� u�ytkownik Kadu po ustawieniu danego,
		a wi�c wy��czaj�c "blokowany" oraz "niedost�pny z opisem".
	**/
	static int initCount();

	/**
		\fn static QString name(int nr)
		Zamienia dany status z postaci indeksowej na form� �a�cucha znak�w. Posta� indeksowa musi mie�ci� si�
		w liczbie zwr�conej przez UserStatus::count() (musi by� przynajmniej o 1 mniejsza od tej liczby).
		\return status w postaci �a�cucha znak�w.
	**/
	static QString name(int nr);

	/**
		\fn QString name() const
		Zamienia aktualny status z postaci indeksowej na form� �a�cucha znak�w.
		\return status w postaci �a�cucha znak�w.
	**/
	QString name() const;

	/**
		\fn void refresh()
		Od�wie�a aktualny status.
	**/
	void refresh();

	/**
		\fn virtual QString protocolName() const;
		Zwraca identyfikator protoko�u.
	**/
	virtual QString protocolName() const;

public slots:
	/**
		\fn void setOnline(const QString& desc = "")
		Ustawia status na "dost�pny" z ewentualnym opisem.
		\param desc opcjonalny opis. Domy�lnie brak.
	**/
	void setOnline(const QString &desc = QString::null);

	/**
		\fn void setBusy(const QString& desc = "")
		Ustawia status na "zaraz wracam" z ewentualnym opisem.
		\param desc opcjonalny opis. Domy�lnie brak.
	**/
	void setBusy(const QString &desc = QString::null);

	/**
		\fn void setInvisible(const QString& desc = "")
		Ustawia status na "ukryty" z ewentualnym opisem.
		\param desc opcjonalny opis. Domy�lnie brak.
	**/
	void setInvisible(const QString &desc = QString::null);

	/**
		\fn void setOffline(const QString& desc = "")
		Ustawia status na "niedost�pny" z ewentualnym opisem.
		\param desc opcjonalny opis. Domy�lnie brak.
	**/
	void setOffline(const QString &desc = QString::null);

	/**
		\fn void setTalkWithMe(const QString& desc = "")
		Ustawia status na "porozmawiaj ze mna" z ewentualnym opisem.
		\param desc opcjonalny opis. Domy�lnie brak.
	**/
	void setTalkWithMe(const QString &desc = QString::null);

	/**
		\fn void setDoNotDisturb(const QString& desc = "")
		Ustawia status na "nie przeszkadzac" z ewentualnym opisem.
		\param desc opcjonalny opis. Domy�lnie brak.
	**/
	void setDoNotDisturb(const QString &desc = QString::null);

	/**
		\fn void setBlocking()
		Ustawia tryb blokuj�cy statusu.
	**/
	void setBlocking();

	/**
		\fn void setDescription(const QString& desc = "")
		Ustawia opis dla statusu.
		\param desc opcjonalny opis. Domy�lnie pusty.
	**/
	void setDescription(const QString &desc = QString::null);

	/**
		\fn void setStatus(const UserStatus& stat)
		Ustawia status na dany.
		\param stat status kt�ry chcemy ustawi�
	**/
	void setStatus(const UserStatus &stat);

	/**
		\fn void setStatus(eUserStatus stat, const QString& desc = "")
		Ustawia status na dany.
		\param stat enumeracyjna posta� statusu.
			\arg \c Online - Dost�pny
			\arg \c Busy - Zaraz wracam
			\arg \c Invisible - Ukryty
			\arg \c Blocking - Blokuj�cy
			\arg \c Offline - Nidost�pny
		\param desc opcjonalny opis. Domy�lnie brak.
	**/
	void setStatus(eUserStatus stat, const QString &desc = QString::null);

	/**
		\fn void setIndex(int index, const QString& desc = "")
		Ustawia status na dany.
		\param index indeksowa posta� statusu.
		\note parzyste indeksy odpowiadaj� statusom bez opis�w.
		\param desc opcjonalny opis. Domy�lnie pusty.
	**/
	void setIndex(int index, const QString &desc = QString::null);

	/**
		\fn void setFriendsOnly(bool f)
		Ustawia tryb "tylko dla znajomych".
		\param f warto�� logiczna informuj�ca o tym, czy status ma mie� w��czony ten tryb, czy nie.
	**/
	void setFriendsOnly(bool f);

signals:
	/**
		\fn void goOnline(const QString& desc)
		Sygna� emitowany, gdy status jest zmieniany na "dost�pny".
		\param desc Opcjonalny opis. Mo�e mie� warto�� "" je�li status jest bezopisowy.
	**/
	void goOnline(const QString &desc);

	/**
		\fn void goBusy(const QString& desc)
		Sygna� emitowany, gdy status jest zmieniany na "zaraz wracam".
		\param desc Opcjonalny opis. Mo�e mie� warto�� "" je�li status jest bezopisowy.
	**/
	void goBusy(const QString &desc);

	/**
		\fn void goInvisible(const QString& desc)
		Sygna� emitowany, gdy status jest zmieniany na "ukryty".
		\param desc Opcjonalny opis. Mo�e mie� warto�� "" je�li status jest bezopisowy.
	**/
	void goInvisible(const QString &desc);

	/**
		\fn void goOffline(const QString& desc)
		Sygna� emitowany, gdy status jest zmieniany na "niedost�pny".
		\param desc Opcjonalny opis. Mo�e mie� warto�� "" je�li status jest bezopisowy.
	**/
	void goOffline(const QString &desc);

	/**
		\fn void goTalkWithMe(const QString& desc)
		Sygna� emitowany, gdy status jest zmieniany na "porozmawiaj ze mna".
		\param desc Opcjonalny opis. Mo�e mie� warto�� "" je�li status jest bezopisowy.
	**/
	void goTalkWithMe(const QString &desc);

	/**
		\fn void goDoNotDisturb(const QString& desc)
		Sygna� emitowany, gdy status jest zmieniany na "nie przeszkadzac".
		\param desc Opcjonalny opis. Mo�e mie� warto�� "" je�li status jest bezopisowy.
	**/
	void goDoNotDisturb(const QString &desc);

	/**
		\fn void goBlocking()
		Sygna� jest emitowany, kiedy tryb blokuj�cy zostaje w��czony w tym statusie.
	**/
	void goBlocking();

	/**
		\fn void changed(const UserStatus& status)
		Sygna� jest emitowany, gdy status zosta� zmieniony na kt�rykolwiek.
		\param status status, na kt�ry dokonano zmiany
		\param oldStatus status, z kt�rego dokonano zmiany
	**/
	void changed(const UserStatus &status, const UserStatus &oldStatus);

};

extern QStringList defaultdescriptions;

#endif
