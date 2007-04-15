#ifndef HINT_H
#define HINT_H

#include <qcolor.h>
#include <qevent.h>
#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmap.h>
#include <qobject.h>
#include <qpair.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qwidget.h>

#include "gadu.h"

class Notification;

class Hint : public QWidget
{
	Q_OBJECT

	private:
		QVBoxLayout *vbox;

		QHBoxLayout *labels;
		QHBoxLayout *callbacksBox;

		QLabel *icon;
		QLabel *label;
		QColor bcolor; //kolor t³a
		unsigned int secs;
		unsigned int startSecs;
		UserListElements users;

		Notification *notification;
		QStringList details;

		bool haveCallbacks;

		void createLabels(const QPixmap &pixmap);
		void updateText();

		void resetTimeout();

	private slots:
		void notificationClosed();

	protected:
		virtual void mousePressEvent(QMouseEvent * event);
		virtual void enterEvent(QEvent *event);
		virtual void leaveEvent(QEvent *event);

	public:
		Hint(QWidget *parent, const QString &text, const QPixmap &pixmap, unsigned int timeout);
		Hint(QWidget *parent, Notification *notification);
		virtual ~Hint();

		/**
			zwraca listê uinów dotycz±cych tego dymka
		**/
		UserListElements getUsers() const { return users; }

		void getData(QString &text, QPixmap &pixmap, unsigned int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor);
		bool requireManualClosing();
		bool isDeprecated();

		void addDetail(const QString &detail);

	public slots:
		/**
			minê³a sekunda, zmniejsza licznik pozosta³ych sekund,
			zwraca true je¿eli jeszcze pozosta³ czas
			false, gdy czas siê skoñczy³
		**/
		void nextSecond();
		/**
			ustawia parametry dymka
			font - czcionka
			fgcolor - kolor czcionki
			bgcolor - kolor t³a
			id - identyfikator dymka
		**/
		void set(const QFont &font, const QColor &fgcolor, const QColor &bgcolor);
		/**
			ustawia listê uinów dotycz±cych tego dymka
		**/
		void setUsers(const UserListElements &i) { users = i; };

		void acceptNotification();
		void discardNotification();

	signals:
		void leftButtonClicked(Hint *hint);
		void rightButtonClicked(Hint *hint);
		void midButtonClicked(Hint *hint);
		void closing(Hint *hint);
};

#endif
