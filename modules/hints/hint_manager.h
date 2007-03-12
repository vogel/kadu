#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include <qframe.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qstring.h>
#include <qfont.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qpoint.h>

#include "gadu.h"
#include "hint.h"
#include "hint_manager_slots.h"
#include "../notify/notify.h"

class HintManager : public Notifier
{
	Q_OBJECT

	private:
		QFrame *frame;
		QVBoxLayout *layout;
		HintManagerSlots *hint_manager_slots;
		QTimer *hint_timer;
		QPtrList<Hint> hints;
		QFrame *tipFrame;

		/**
			ustala róg, od którego bêdzie liczona pozycja grupy dymków
		**/
		void setLayoutDirection();

		void processButtonPress(const QString &buttonName, Hint *hint);

		void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, UserListElements senders, const QString &msg);

	private slots:
		/**
			minê³a sekunda: aktualizuje czasy wszystkich dymków
			usuwa te, którym min±³ czas
		**/
		void oneSecond();

		/**
			przesuwa ca³± grupê hintów zgodnie z preferowanym rozmiarem grupy
			i wykryt±/okre¶lon± w konfiguracji pozycj± traya
		**/
		void setHint();

		/**
			na dymku o numerze id klikniêto lewym przyciskiem myszy
			w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
		**/
		void leftButtonSlot(Hint *hint);

		/**
			na dymku o numerze id klikniêto prawym przyciskiem myszy
			w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
		**/
		void rightButtonSlot(Hint *hint);

		/**
			na dymku o numerze id klikniêto ¶rodkowym przyciskiem myszy
			w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
		**/
		void midButtonSlot(Hint *hint);

		void deleteHint(Hint *hint);
		void deleteHintAndUpdate(Hint *hint);

	public slots:
/*********** sloty dla notify ************/
		/* nowa rozmowa */
		void newChat(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);

		/* nowa wiadomo¶æ w oknie chat */
		void newMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab);

		/* b³±d po³±czenia */
		void connectionError(Protocol *protocol, const QString &message);

		/* u¿ytkownik zmieni³ status */
		void userStatusChanged(UserListElement ule, QString protocolName, const UserStatus &oldStatus);

		/* u¿ytkownik zmieni³ status na "Dostêpny" */
		void userChangedStatusToAvailable(const QString &protocolName, UserListElement);

		/* u¿ytkownik zmieni³ status na "Zaraz wracam" */
		void userChangedStatusToBusy(const QString &protocolName, UserListElement);

		/* u¿ytkownik zmieni³ status na "Niewidoczny" */
		void userChangedStatusToInvisible(const QString &protocolName, UserListElement);

		/* u¿ytkownik zmieni³ status na "Niewidoczny" lub "Niedostêpny" */
		void userChangedStatusToNotAvailable(const QString &protocolName, UserListElement);

		void userBoxChangeToolTip(const QPoint &point, UserListElement user, bool show);

		/* inna informacja do powiadomienia */
		/* je¿eli parameters == NULL, to brane s± domy¶lne ustawienia dymków typu message
		 * w przeciwnym razie z mapy brane s± nastêpuj±ce warto¶ci:
		 *		"Pixmap"           - QPixmap
		 *		"Font"             - QFont
		 *		"Foreground color" - QColor
		 *		"Background color" - QColor
		 *		"Timeout"          - unsigned int
		 *		"ShowSource"       - bool
		 * je¿eli która¶ z warto¶ci jest nieokre¶lona, to dla niej brana jest warto¶æ domy¶lna
		 * gdy UserListElement != NULL, to jego uin brany jest do tworzenia dymka (LMB np otwiera okno rozmowy z danym uinem)
		 */
		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
		virtual void externalEvent(Notification *notification);
/*********** koniec slotów dla notify *************/

		/**
			pokazuje nowy dymek o tre¶ci "text", obrazku "pixmap",
			czcionk± "font" o kolorze "fgcolor", tle "bgcolor",
			czasie zaniku "timeout" (sekund), dotycz±cy "senders"

			pozosta³e funkcje pokazuj±ce dymki s± tylko opakowaniami na t± funkcjê
		**/
		void addHint(const QString &text, const QPixmap &pixmap, const QFont &font, const QColor &fgcolor, const QColor &bgcolor, unsigned int timeout, const UserListElements &senders = UserListElements());
		void addHint(Notification *notification);

		/**
			je¿eli dymek dotyczy³ konkrentej osoby lub grupy osób, to otwierane jest okno chatu
			dymek jest kasowany
		**/
		void openChat(Hint *hint);

		/**
			usuwa wszystkie dymki
		**/
		void deleteAllHints();


	signals:
		/**
			HintManager szuka pozycji traya, aby wy¶wietliæ w jego okolicy dymki
		**/
		void searchingForTrayPosition(QPoint& pos);

	public:
		HintManager(QWidget *parent=0, const char *name=0);
		~HintManager();

		virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
};

extern HintManager *hint_manager;

#endif
