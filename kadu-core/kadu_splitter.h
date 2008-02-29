#ifndef KADU_SPLITTER_H
#define KADU_SPLITTER_H

#include <qglobal.h>

#include <q3valuelist.h>
#include <qsplitter.h>
//Added by qt3to4:
#include <QChildEvent>

class QPainter;
class QChildEvent;
class QWidget;
class KaduTextBrowser;

/**
	Klasa wykorzystana do oddzielenia listy u¿ytkowników od historii
	i pola do wpisywania wiadomo¶ci
	\class KaduSplitter
	\brief Klasa rozdzielaj±ca kontrolki

**/
class KaduSplitter : public QSplitter
{
	protected:
		QList<KaduTextBrowser*> textbrowsers;
// 		void drawContents(QPainter* p);
		void childEvent(QChildEvent* c);

	public:
	/**
		Konstruktor tworz±cy obiekt
		\fn KaduSplitter(QWidget* parent = 0, const char* name = 0)
		\param parent rodzic okna
		\param name nazwa obiektu

	**/
		KaduSplitter(QWidget* parent = 0, const char* name = 0);

	/**
		Konstruktor tworz±cy obiekt o danej orientacji
		\fn KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0)
		\param parent rodzic okna
		\param name nazwa obiektu
		\param o orientacja separatora
	**/
		KaduSplitter(Qt::Orientation o, QWidget* parent = 0, const char* name = 0);
};

#endif
