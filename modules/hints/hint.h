#ifndef HINT_H
#define HINT_H

#include <qobject.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qfont.h>
#include <qevent.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlayout.h>

#include "gadu.h"

class Hint : public QHBoxLayout
{
	Q_OBJECT

	private:
		QLabel *icon;
		QLabel *label;
		QColor bcolor; //kolor t³a
		unsigned int secs;
		unsigned int ident; //identyfikator
		UinsList uins;
		void enter();//rozja¶nia dymek (t³o)
		void leave();//ustawia domy¶lny kolor t³a

	public:
		Hint(QWidget *parent, const QString &text, const QPixmap &pixmap, unsigned int timeout);
		~Hint();
		/**
			zwraca listê uinów dotycz±cych tego dymka
		**/
		UinsList getUins() const { return uins; }
		
		/**
			zwraca identyfikator dymka
		**/
		unsigned int id() const {return ident;}
		
		void getData(QString &text, QPixmap &pixmap, unsigned int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor);

	protected:
		/**
			przy najechaniu myszk± uruchamia enter(), przy opuszczeniu myszk± dymka uruchamia leave()
			przy naci¶niêciu którego¶ przycisku myszy, emituje odpowiedni sygna³ *ButtonClicked(uint)
		**/
		bool eventFilter(QObject *obj, QEvent *ev);		

	public slots:
		/**
			minê³a sekunda, zmniejsza licznik pozosta³ych sekund,
			zwraca true je¿eli jeszcze pozosta³ czas
			false, gdy czas siê skoñczy³
		**/
		bool nextSecond();
		/**
			gdy show==true pokazuje dymek
		**/
		void setShown(bool show);
		/**
			ustawia parametry dymka
			font - czcionka
			fgcolor - kolor czcionki
			bgcolor - kolor t³a
			id - identyfikator dymka
			show - czy pokazaæ (teraz)
		**/
		void set(const QFont &font, const QColor &fgcolor, const QColor &bgcolor, unsigned int id, bool show = true);
		/**
			ustawia identyfikator
		**/
		void setId(unsigned int id) {ident = id;}
		/**
			ustawia listê uinów dotycz±cych tego dymka
		**/
		void setUins(const UinsList &i) { uins=i; };

	signals:
		void leftButtonClicked(unsigned int id);
		void rightButtonClicked(unsigned int id);
		void midButtonClicked(unsigned int id);
};

#endif
