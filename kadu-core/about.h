#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>

/**
	\class About
	\brief Okno z informacjami o programie.
**/
class About : public QWidget {
	Q_OBJECT

	QString loadFile(const QString &name);
	void keyPressEvent(QKeyEvent *);

public:
	/**
		\fn About(QWidget *parent = 0)
		Konstruktor tworzy okno, ustawia zak³adki z informacjami o autorach, podziêkowaniach, licencji oraz listê zmian w programie. \n
		Domy¶lnie okno ma rozmiar 640x420.
		\param parent rodzic okna
		\param name nazwa obiektu
	**/
	About(QWidget *parent = 0);

	/**
		\fn ~About()
		Destruktor zapisuje rozmiar okna.
	**/
	~About();

};

#endif
