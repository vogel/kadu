#ifndef ABOUT_H
#define ABOUT_H

#include <qhbox.h>

/**
	\class About 
	\brief Okno z informacjami o programie.
**/
class About : public QHBox {
	Q_OBJECT

	public:
	
	/**
		\fn About(QWidget *parent=NULL, const char *name=NULL)
		Konstruktor tworzy okno, ustawia zak³adki z informacjami o autorach, podziêkowaniach, licencji oraz listê zmian w programie. \n
		Domy¶lnie okno ma rozmiar 640x420.
		\param parent rodzic okna
		\param name nazwa obiektu
	**/
		About(QWidget *parent=NULL, const char *name=NULL);
	
	/**
		\fn ~About()
		Destruktor zapisuje rozmiar okna.
	**/
		~About();

	private:
	/**
		\fn QString loadFile(const QString &name)
		Funkcja zwracaj±ca zawarto¶æ pliku.
		\param name nazwa pliku do przeczytania
	**/
		QString loadFile(const QString &name);
	
	/**
		\fn void keyPressEvent(QKeyEvent *)
		Funkcja obs³uguj±ca zdarzenie naci¶niêcia przycisku 
		Esc do zamkniêcia okna.
	**/
		void keyPressEvent(QKeyEvent *);
};

#endif
