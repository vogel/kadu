#ifndef KADU_COLOR_SELECTOR_H
#define KADU_COLOR_SELECTOR_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qpushbutton.h>
#include <qcolor.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QCloseEvent>

class QCloseEvent;

extern const char colors[16][8];

/**
	\class ColorSelectorButton
	\brief Klasa do wyboru koloru za pomoc± przycisku
**/
class ColorSelectorButton : public QPushButton
{
	Q_OBJECT

	private:
		QColor color; /*!< kolor czcionki */

	private slots:
		/**
			\fn void buttonClicked()
			Slot obs³uguj±cy naci¶niêcie przycisku
		**/
		void buttonClicked();

	public:
		/**
			\fn ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width=1, const char *name=0)
			Konstruktor tworz±cy okno do wyboru koloru
			\param parent rodzic okna
			\param qcolor kolor
			\param width
			\param name nazwa obiektu
		**/
		ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width=1, const char *name=0);

	signals:
		/**
			\fn void clicked(const QColor& color)
			Sygna³ zostaje wys³any po wybraniu koloru
		**/
		void clicked(const QColor& color);
};

/**
	\class ColorSelector
	\brief Klasa do wyboru kolorów
**/
class ColorSelector : public QWidget
{
	Q_OBJECT

	private slots:
		/**
			\fn void iconClicked(const QColor& color);
			Slot obs³uguj±cy wybranie koloru
			\param color wybrany kolor
		**/
		void iconClicked(const QColor& color);

	protected:
		/**
			\fn void closeEvent(QCloseEvent*)
			Funkcja obs³uguj±ca zamkniêcie okna wyboru kolorów
		**/
		void closeEvent(QCloseEvent*);

	public:
		/**
			\fn ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0)
			Konstruktor obiektu do wyboru kolorów
			\param defColor
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0);

	public slots:
		/**
			\fn void alignTo(QWidget* w)
		  	Slot wyrównuje pozycjê do widgeta
			\param w wka¼nik do obiektu ktorego bêdziemy wyrównywac
		**/
		void alignTo(QWidget* w);

	signals:
		/**
			\fn void aboutToClose()
			Sygna³ jest emitowany gdy zamykany jest wybór kolorów
		**/
		void aboutToClose();

		/**
			\fn void colorSelect(const QColor&)
			Sygna³ jest emitowany gdy wybrano kolor

		**/
		void colorSelect(const QColor&);
};

#endif
