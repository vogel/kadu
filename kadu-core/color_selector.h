#ifndef KADU_COLOR_SELECTOR_H
#define KADU_COLOR_SELECTOR_H

#include <QPushButton>

extern const char colors[16][8];

/**
	\class ColorSelectorButton
	\brief Klasa do wyboru koloru za pomoc± przycisku
**/
class ColorSelectorButton : public QPushButton
{
	Q_OBJECT

	QColor color;

private slots:
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
	ColorSelectorButton(const QColor &qcolor, int width, QWidget *parent);

signals:
	// TODO: rename
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
	void iconClicked(const QColor &color);

protected:
	/**
		\fn void closeEvent(QCloseEvent*)
		Funkcja obs³uguj±ca zamkniêcie okna wyboru kolorów
	**/
	void closeEvent(QCloseEvent *);

public:
	/**
		\fn ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0)
		Konstruktor obiektu do wyboru kolorów
		\param defColor
		\param parent rodzic okna
		\param name nazwa obiektu
	**/
	ColorSelector(const QColor &defColor, QWidget *parent = 0);

public slots:
	// TODO: WTF? remove!!
	/**
		\fn void alignTo(QWidget* w)
	  	Slot wyrównuje pozycjê do widgeta
		\param w wka¼nik do obiektu ktorego bêdziemy wyrównywac
	**/
	void alignTo(QWidget *w);

signals:
	// TODO: rename
	/**
		\fn void aboutToClose()
		Sygna³ jest emitowany gdy zamykany jest wybór kolorów
	**/
	void aboutToClose();

	// TODO: rename
	/**
		\fn void colorSelect(const QColor&)
		Sygna³ jest emitowany gdy wybrano kolor
	**/
	void colorSelect(const QColor &);

};

#endif
