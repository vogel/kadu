#ifndef MISC_H
#define MISC_H

#include <qvaluelist.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qcstring.h>
#include <qdatetime.h>
#include <qkeysequence.h>
#include <qlineedit.h>
#include <libintl.h>


#include "libgadu.h"
#include "userlist.h"

#define __c2q(__char_pointer__) QString::fromLocal8Bit(__char_pointer__)

/*
	Zmienia sciezke relatywna do katalogu z ustawieniami gg
	na sciezke bezwzgledna uwzgledniajac zmienne srodowiskowe
	$HOME i $CONFIG_DIR
*/
QString ggPath(QString subpath);

#define i18n(String) __c2q(gettext(String))

QString cp2unicode(unsigned char *);
QCString unicode2cp(const QString &);
QPixmap loadIcon(const QString &filename);
QString printDateTime(const QDateTime &datetime);
QString timestamp(time_t = 0);
QDateTime currentDateTime();
QString pwHash(const QString tekst);
void escapeSpecialCharacters(QString &msg);
QString formatGGMessage(const QString &msg, int formats_length, void *formats);
QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats);
QString parse(QString s, UserListElement ule, bool escape = true);

class UinsList : public QValueList<uin_t>
{
	public:
		UinsList();
		bool equals(UinsList &uins);
		void sort();
};

class ChooseDescription : public QDialog {
	Q_OBJECT
	public:
		ChooseDescription ( int nr, QWidget * parent=0, const char * name=0);

	private:
		QComboBox *desc;
		QLabel *l_yetlen;

	private slots:
		void okbtnPressed();
		void cancelbtnPressed();
		void updateYetLen(const QString&);

};

struct iconhandle {
	QString name;
	QPixmap pixmap;
};

class IconsManager {
	public:
		IconsManager();
		IconsManager(QString &dir);
		void setDirectory(QString &dir);
		QPixmap *loadIcon(QString name);
		void clear();

	private:
		QValueList<iconhandle> icons;
		QString directory;
};

extern IconsManager *icons;



class HotKey : public QLineEdit
{
public:
    HotKey::HotKey(QWidget *parent =0,const char* name =0);
    static QString keyEventToString(QKeyEvent *e);    
    static QKeySequence shortCutFromFile(const QString &name);
    static bool shortCut(QKeyEvent *e,const QString &name);    
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

};

/**
	Klasa reprezentuj±ca dokument html. Przechowuje
	listê elementów. Element mo¿e byæ tagiem html
	albo kawa³kiem tekstu.
**/
class HtmlDocument
{
	private:
		struct Element
		{
			QString text;
			bool tag;
		};
		QValueList<Element> Elements;
		void escapeText(QString& text);
		void unescapeText(QString& text);
		void addElement(Element e);

	public:
		/**
			Parsuje podany napis zgodny ze struktur± html
			i otrzymane dane przypisuje do dokumentu.
			@param html napis do skonwertowania
		**/
		void parseHtml(const QString& html);
		/**
			Na podstawie zawarto¶ci dokumentu generuje
			napis zgodny ze struktur± html. Znaki specjalne
			wchodz±ce w sk³ad elementów nie bêd±cych tagami
			html s± escapowane.
		**/
		QString generateHtml();
		/**
			Zwraca ilo¶æ elementów wchodz±cych w sk³ad
			dokumentu.
		**/
		int countElements();
		/**
			Sprawdza czy element o podanym indeksie jest
			tagiem html czy zwyk³ym tekstem.
		**/
		bool isTagElement(int index);
		/**
			Zwraca tekst elementu o podanym indeksie.
		**/
		QString elementText(int index);
		/**
			Ustawia tekst i typ elementu o podanym indeksie.
		**/			
		void setElementValue(int index,const QString& text,bool tag=false);
		/**
			Wydziela z elementu podany fragment tekstu.
			Element mo¿e pozostaæ jeden albo
			ulec podzieleniu na dwa lub nawet trzy elementy.
			Funkcja zmienia warto¶æ argumentu index, aby
			indeks wydzielonego elementu by³ aktualny.
		**/
		void splitElement(int& index,int start,int length);
};

#endif
