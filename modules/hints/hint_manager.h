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

class HintManager : public QFrame
{
	Q_OBJECT

	private:
		HintManagerSlots *hint_manager_slots;
		QTimer *hint_timer;
		QGridLayout *grid;
		QPtrList<Hint> hints;
		
		/**
			ustala róg, od którego bêdzie liczona pozycja grupy dymków
		**/
		void setGridOrigin();

	public:
		HintManager();
		~HintManager();

	public slots:
		/**
			pokazuje nowy dymek o tre¶ci "text", obrazku "pixmap",
			czcionk± "font" o kolorze "fgcolor", tle "bgcolor",
			czasie zaniku "timeout" (sekund), dotycz±cy "senders"
			
			pozosta³e funkcje pokazuj±ce dymki s± tylko opakowaniami na t± funkcjê
		**/
		void addHint(const QString &text, const QPixmap &pixmap, const QFont &font, const QColor &fgcolor, const QColor &bgcolor, unsigned int timeout, UinsList* senders=0);
		
		/**
			usuwa dymek o numerze id
		**/
		void deleteHint(unsigned int id);
		
		/**
			je¿eli dymek dotyczy³ konkrentej osoby lub grupy osób, to otwierane jest okno chatu
			dymek jest kasowany
		**/
		void openChat(unsigned int id);
		
		/**
			usuwa wszystkie dymki
		**/
		void deleteAllHints();

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
		void leftButtonSlot(unsigned int id);
		
		/**
			na dymku o numerze id klikniêto prawym przyciskiem myszy
			w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
		**/
		void rightButtonSlot(unsigned int id);
		
		/**
			na dymku o numerze id klikniêto ¶rodkowym przyciskiem myszy
			w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
		**/
		void midButtonSlot(unsigned int id);
	
	public slots:
/*********** sloty dla notify ************/
		/* nowa rozmowa */
		void newChat(UinsList senders, const QString& msg, time_t time);
	
		/* nowa wiadomo¶æ w oknie chat */
		void newMessage(UinsList senders, const QString& msg, time_t time, bool &grab);
	
		/* b³±d po³±czenia */
		void connectionError(const QString &message);
	
		/* u¿ytkownik ZMIENIA status (mo¿na operowaæ tylko na podanych parametrach, bo status _jeszcze_ siê nie zmieni³) */
		void userChangingStatus(const UinType uin, const unsigned int oldstatus, const unsigned int status);
	
		/* u¿ytkownik ZMIENI£ status na "Dostêpny" */
		void userChangedStatusToAvailable(const UserListElement &ule);
	
		/* u¿ytkownik zmieni³ status na "Zaraz wracam" */
		void userChangedStatusToBusy(const UserListElement &ule);
	
		/* u¿ytkownik zmieni³ status na "Niewidoczny" lub "Niedostêpny" */
		void userChangedStatusToNotAvailable(const UserListElement &ule);

		/* inna informacja do powiadomienia */
		/* je¿eli parameters == NULL, to brane s± domy¶lne ustawienia dymków typu message
		 * w przeciwnym razie z mapy brane s± nastêpuj±ce warto¶ci:
		 *		"Pixmap"           - QPixmap
		 *		"Font"             - QFont
		 *		"Foreground color" - QColor
		 *		"Background color" - QColor
		 *		"Timeout"          - unsigned int
		 * je¿eli która¶ z warto¶ci jest niekre¶lona, to dla niej brana jest warto¶æ domy¶lna
		 * gdy UserListElement != NULL, to jego uin brany jest do tworzenia dymka (LMB np otwiera okno rozmowy z danym uinem)
		 */
		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
/*********** koniec slotów dla notify *************/

	signals:
		/**
			HintManager szuka pozycji traya, aby wy¶wietliæ w jego okolicy dymki
		**/
		void searchingForTrayPosition(QPoint& pos);
};

extern HintManager *hint_manager;

#endif
