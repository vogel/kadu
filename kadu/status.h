#ifndef KADU_STATUS_H
#define KADU_STATUS_H

#include <qobject.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qstringlist.h>

/**
	\enum eUserStatus
	Opisuje stan statusu.
**/
enum eUserStatus
{
	Online, /*!< Dostêpny */
	Busy, /*!< Zaraz wracam */
	Invisible, /*!< Ukryty */
	Offline, /*!< Niedostêpny */
	Blocking /*!< Blokuj±cy */
};

/**
	Klasa ta zawiera informacje o statusie kontaktu lub u¿ytkownika Kadu, oraz metody
	s³u¿±ce do zmiany tego statusu.
	\class UserStatus
	\brief Klasa opisuj±ca status.
**/
class UserStatus : public QObject
{
	Q_OBJECT

	private:
		bool Changed;

	protected:
		/**
			\var eUserStatus Stat
			Zawiera informacjê o aktualnym statusie w postaci enumerycznej.
		**/
		eUserStatus Stat;

		/**
			\var QString Description
			Zawiera aktualny opis.
		**/
		QString Description;

		/**
			\var bool FriendsOnly
			Zawiera informacjê o trybie "tylko dla znajomych".
		**/
		bool FriendsOnly;

	public:
		/**
			\fn UserStatus()
			Standardowy konstruktor.
		**/
		UserStatus();

		/**
			\fn UserStatus(const UserStatus &copyMe)
			Konstruktor kopiuj±cy.
			\param copyMe obiekt który chcemy skopiowaæ.
		**/
		UserStatus(const UserStatus &copyMe);

		/**
			\fn void operator = (const UserStatus &copyMe)
			Operator kopiuj±cy.
			\param copyMe obiekt który chcemy skopiowaæ.
		**/
		void operator = (const UserStatus &copyMe);

		/**
			\fn bool operator == (const UserStatus &compare) const
			Operator porównuj±cy. Sprawdza to¿samo¶æ obiektów.
			\param compare obiekt z którym chcemy porównaæ bierz±cy obiekt.
		**/
		bool operator == (const UserStatus &compare) const;

		/**
			\fn bool operator != (const UserStatus &compare) const
			Operator porównuj±cy. Sprawdza ró¿nicê pomiêdzy obiektami.
			\param compare obiekt z którym chcemy porównaæ bierz±cy obiekt.
		**/
		bool operator != (const UserStatus &compare) const;

		/**
			\fn virtual UserStatus *copy() const
			Wykonuje kopiê obiektu, któr± pó¼niej nale¿y samodzielnie zniszczyæ
		**/
		virtual UserStatus *copy() const;

		virtual ~UserStatus();

		/**
			\fn bool isOnline() const
			Sprawdza czy kontakt jest "dostêpny".
			\return TRUE je¶li jest "dostêpny", FALSE w przeciwnym wypadku.
		**/
		bool isOnline() const;

		/**
			\fn bool isBusy() const
			Sprawdza czy kontakt ma status "zaraz wracam".
			\return TRUE je¶li ma status "zaraz wracam", FALSE w przeciwnym wypadku.
		**/
		bool isBusy() const;

		/**
			\fn bool isInvisible() const
			Sprawdza czy kontakt jest "ukryty".
			\return TRUE je¶li jest "ukryty", FALSE w przeciwnym wypadku.
		**/
		bool isInvisible() const;

		/**
			\fn bool isOffline() const
			Sprawdza czy kontakt jest "niedostêpny".
			\return TRUE je¶li jest "niedostêpny", FALSE w przeciwnym wypadku.
		**/
		bool isOffline() const;

		/**
			\fn bool isBlocking() const
			Sprawdza czy kontakt jest "blokuj±cy".
			\return TRUE je¶li jest "blokuj±cy", FALSE w przeciwnym wypadku.
		**/
		bool isBlocking() const;

		/**
			\fn static bool isOffline(int index)
			Sprawdza czy status dany w postaci indeksowej jest statusem "niedostêpny" lub jego
			odpowiednikiem z opisem.
			\return TRUE je¶li status oznacza "niedostêpny", FALSE w przeciwnym wypadku.
		**/
		static bool isOffline(int index);

		/**
			\fn bool hasDescription() const
			Sprawdza czy status jest z opisem.
			\return TRUE gdy status jest z opisem, FALSE w przeciwnym wypadku.
		**/
		bool hasDescription() const;

		/**
			\fn bool isFriendsOnly() const
			Sprawdza czy status jest "tylko dla znajomych".
			\return TRUE je¶li jest "tylko dla znajomych", FALSE w przeciwnym wypadku.
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
			Zamienia enumeracyjn± postaæ statusu na indeksow±.
			\param stat enumeracyjna postaæ statusu.
			\param has_desc informuje, czy jest to odmiana statusu z opisem.
			\return status w postaci indeksowej.
		**/
		static int index(eUserStatus stat, bool has_desc);

		/**
			\fn virtual QPixmap pixmap(bool mobile = false) const
			Zwraca ikonê reprezentuj±c± ten status.
			\param mobile informuje czy ikona ma zawieraæ element telefonu komórkowego. Domy¶lnie FALSE.
			\return pixmapê reprezentuj±c± ikonê.
		**/
		virtual QPixmap pixmap(bool mobile = false) const;

		/**
			\fn virtual QPixmap pixmap(const UserStatus &stat, bool mobile = false) const
			Zwraca ikonê reprezentuj±c± dany status.
			\param stat status którego ikonê nale¿y zwróciæ
			\param mobile informuje czy ikona ma zawieraæ element telefonu komórkowego. Domy¶lnie FALSE.
			\return pixmapê reprezentuj±c± ikonê.
		**/
		virtual QPixmap pixmap(const UserStatus &, bool mobile = false) const;

		/**
			\fn virtual QPixmap pixmap(eUserStatus stat, bool has_desc, bool mobile = false) const
			Zwraca ikonê reprezentuj±c± dany status.
			\param stat status w postaci enumeracyjnej którego ikonê nale¿y zwróciæ
			\param has_desc informuje, czy jest to odmiana statusu z opisem.
			\param mobile informuje czy ikona ma zawieraæ element telefonu komórkowego. Domy¶lnie FALSE.
			\return pixmapê reprezentuj±c± ikonê.
		**/
		virtual QPixmap pixmap(eUserStatus stat, bool has_desc, bool mobile = false) const;

		/**
			\fn static eUserStatus fromString(const QString& stat)
			Zamienia status z formy ³añcucha znaków na postaæ enumeracyjn±.
			\param stat ³añcuch znaków reprezentuj±cy nazwê statusu. Poprawne nazwy statusów to:
				\arg \c Online
				\arg \c Busy
				\arg \c Invisible
				\arg \c Blocking
			\return enumeracyjna postaæ statusu.
		**/
		static eUserStatus fromString(const QString& stat);

		/**
			\fn static QString toString(eUserStatus stat, bool has_desc)
			Zamienia status z postaci enumeracyjnej na formê ³añcucha znaków.
			\param stat postaæ enumeracyjna statusu
			\param has_desc informuje, czy jest to odmiana statusu z opisem.
			\return enumeracyjna postaæ statusu.
		**/
		static QString toString(eUserStatus stat, bool has_desc);

		/**
			\fn QString toString(eUserStatus stat, bool has_desc)
			Zamienia status na formê ³añcucha znaków.
			robi dok³adnie tyle co UserStatus::toString(this->status(), this->hasDescription())
			\see toString(eUserStatus, bool)
			\return status w postaci napisu.
		**/
		QString toString() const;

		/**
			\fn static int count()
			\return liczbê wszystkich mo¿liwych statusów, ³±cznie z "blokowany" oraz "niedostêpny z opisem".
		**/
		static int count();

		/**
			\fn static int count()
			\return liczbê wszystkich mo¿liwych statusów, które mo¿e zobaczyæ u¿ytkownik Kadu po ustawieniu danego,
				a wiêc wy³±czaj±c "blokowany" oraz "niedostêpny z opisem".
		**/
		static int initCount();

		/**
			\fn static QString name(int nr)
			Zamienia dany status z postaci indeksowej na formê ³añcucha znaków. Postaæ indeksowa musi mie¶ciæ siê
			w liczbie zwróconej przez UserStatus::count() (musi byæ przynajmniej o 1 mniejsza od tej liczby).
			\return status w postaci ³añcucha znaków.
		**/
		static QString name(int nr);

		/**
			\fn QString name() const
			Zamienia aktualny status z postaci indeksowej na formê ³añcucha znaków.
			\return status w postaci ³añcucha znaków.
		**/
		QString name() const;

		/**
			\fn void refresh()
			Od¶wie¿a aktualny status.
		**/
		void refresh();

		/**
			\fn virtual QString protocolName() const;
			Zwraca identyfikator protoko³u.
		**/
		virtual QString protocolName() const;

	public slots:
		/**
			\fn void setOnline(const QString& desc = "")
			Ustawia status na "dostêpny" z ewentualnym opisem.
			\param desc opcjonalny opis. Domy¶lnie brak.
		**/
		void setOnline(const QString& desc = "");

		/**
			\fn void setBusy(const QString& desc = "")
			Ustawia status na "zaraz wracam" z ewentualnym opisem.
			\param desc opcjonalny opis. Domy¶lnie brak.
		**/
		void setBusy(const QString& desc = "");

		/**
			\fn void setInvisible(const QString& desc = "")
			Ustawia status na "ukryty" z ewentualnym opisem.
			\param desc opcjonalny opis. Domy¶lnie brak.
		**/
		void setInvisible(const QString& desc = "");

		/**
			\fn void setOffline(const QString& desc = "")
			Ustawia status na "niedostêpny" z ewentualnym opisem.
			\param desc opcjonalny opis. Domy¶lnie brak.
		**/
		void setOffline(const QString& desc = "");

		/**
			\fn void setBlocking()
			Ustawia tryb blokuj±cy statusu.
		**/
		void setBlocking();

		/**
			\fn void setDescription(const QString& desc = "")
			Ustawia opis dla statusu.
			\param desc opcjonalny opis. Domy¶lnie pusty.
		**/
		void setDescription(const QString& desc = "");

		/**
			\fn void setStatus(const UserStatus& stat)
			Ustawia status na dany.
			\param stat status który chcemy ustawiæ
		**/
		void setStatus(const UserStatus& stat);

		/**
			\fn void setStatus(eUserStatus stat, const QString& desc = "")
			Ustawia status na dany.
			\param stat enumeracyjna postaæ statusu.
				\arg \c Online - Dostêpny
				\arg \c Busy - Zaraz wracam
				\arg \c Invisible - Ukryty
				\arg \c Blocking - Blokuj±cy
				\arg \c Offline - Nidostêpny
			\param desc opcjonalny opis. Domy¶lnie brak.
		**/
		void setStatus(eUserStatus stat, const QString& desc = "");

		/**
			\fn void setIndex(int index, const QString& desc = "")
			Ustawia status na dany.
			\param index indeksowa postaæ statusu.
			\note parzyste indeksy odpowiadaj± statusom bez opisów.
			\param desc opcjonalny opis. Domy¶lnie pusty.
		**/
		void setIndex(int index, const QString& desc = "");

		/**
			\fn void setFriendsOnly(bool f)
			Ustawia tryb "tylko dla znajomych".
			\param f warto¶æ logiczna informuj±ca o tym, czy status ma mieæ w³±czony ten tryb, czy nie.
		**/
		void setFriendsOnly(bool f);

	signals:
		/**
			\fn void goOnline(const QString& desc)
			Sygna³ emitowany, gdy status jest zmieniany na "dostêpny".
			\param desc Opcjonalny opis. Mo¿e mieæ warto¶æ "" je¶li status jest bezopisowy.
		**/
		void goOnline(const QString& desc);

		/**
			\fn void goBusy(const QString& desc)
			Sygna³ emitowany, gdy status jest zmieniany na "zaraz wracam".
			\param desc Opcjonalny opis. Mo¿e mieæ warto¶æ "" je¶li status jest bezopisowy.
		**/
		void goBusy(const QString& desc);

		/**
			\fn void goInvisible(const QString& desc)
			Sygna³ emitowany, gdy status jest zmieniany na "ukryty".
			\param desc Opcjonalny opis. Mo¿e mieæ warto¶æ "" je¶li status jest bezopisowy.
		**/
		void goInvisible(const QString& desc);

		/**
			\fn void goOffline(const QString& desc)
			Sygna³ emitowany, gdy status jest zmieniany na "niedostêpny".
			\param desc Opcjonalny opis. Mo¿e mieæ warto¶æ "" je¶li status jest bezopisowy.
		**/
		void goOffline(const QString& desc);

		/**
			\fn void goBlocking()
			Sygna³ jest emitowany, kiedy tryb blokuj±cy zostaje w³±czony w tym statusie.
		**/
		void goBlocking();

		/**
			\fn void changed(const UserStatus& status)
			Sygna³ jest emitowany, gdy status zosta³ zmieniony na którykolwiek.
			\param status status, na który dokonano zmiany
			\param oldStatus status, z którego dokonano zmiany
		**/
		void changed(const UserStatus &status, const UserStatus &oldStatus);
};

extern QStringList defaultdescriptions;

#endif
