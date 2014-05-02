#ifndef KADU_COLOR_SELECTOR_H
#define KADU_COLOR_SELECTOR_H

#include <QtWidgets/QPushButton>

extern const char colors[16][8];

/**
	\class ColorSelectorButton
	\brief Klasa do wyboru koloru za pomoc� przycisku
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
		Konstruktor tworz�cy okno do wyboru koloru
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
		Sygna� zostaje wys�any po wybraniu koloru
	**/
	void clicked(const QColor& color);

};

/**
	\class ColorSelector
	\brief Klasa do wyboru kolor�w
**/
class ColorSelector : public QWidget
{
	Q_OBJECT

private slots:
	/**
		\fn void iconClicked(const QColor& color);
		Slot obs�uguj�cy wybranie koloru
		\param color wybrany kolor
	**/
	void iconClicked(const QColor &color);

public:
	/**
		\fn ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0)
		Konstruktor obiektu do wyboru kolor�w
		\param defColor
		\param activatingWidget okno wywo�uj�ce
		\param parent rodzic okna
	**/
	ColorSelector(const QColor &defColor, const QWidget *activatingWidget, QWidget *parent = 0);

signals:
	// TODO: rename
	/**
		\fn void colorSelect(const QColor&)
		Sygna� jest emitowany gdy wybrano kolor
	**/
	void colorSelect(const QColor &);

};

#endif
