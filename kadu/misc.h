#ifndef KADU_MISC_H
#define KADU_MISC_H

#include <qcstring.h>
#include <qdatetime.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qiconset.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qsocket.h>
#include <qstring.h>
#include <qtextbrowser.h>
#include <qtooltip.h>
#include <qvaluelist.h>
#include <qvariant.h>

#include <inttypes.h>

#include "config_file.h"
#include "gadu.h"

class QComboBox;
class QLineEdit;
class QMenuData;

/*
	Zmienia ¶cie¿kê relatywn± do katalogu z ustawieniami gg
	na ¶cie¿kê bezwzgledn± uwzglêdniaj±c zmienne ¶rodowiskowe
	$HOME i $CONFIG_DIR
*/
QString ggPath(const QString &subpath="");

/*
	zwraca ¶cie¿kê do pliku f
	je¿eli drugi parametr nie jest == 0, to funkcja próbuje najpierw ustaliæ
	¶cie¿kê na podstawie argv0, które ma byæ równe argv[0] oraz zmiennej PATH
*/
QString dataPath(const QString &f="", const char *argv0=0);

class UserListElement;

QString cp2unicode(const unsigned char *);
QCString unicode2cp(const QString &);
QString latin2unicode(const unsigned char *);
QCString unicode2latin(const QString &);
QString unicode2std(const QString &);

//zamienia kodowanie polskich znaków przekonwertowane z utf-8 przy pomocy QUrl::encode na kodowanie latin-2
QString unicodeUrl2latinUrl(const QString &buf);
//zamienia polskie znaki na format latin2 "url" (czyli do postaci %XY)
QString unicode2latinUrl(const QString &buf);

QString printDateTime(const QDateTime &datetime);
QString timestamp(time_t = 0);
QDateTime currentDateTime();
QString pwHash(const QString &tekst);
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);
void openWebBrowser(const QString &link);
QString formatGGMessage(const QString &msg, int formats_length, void *formats, UinType sender);
QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats);
QString parse(const QString &s, const UserListElement &ule, bool escape = true);
void stringHeapSort(QStringList &c);
QStringList toStringList(const QString &e1, const QString &e2=QString(), const QString &e3=QString(), const QString &e4=QString(), const QString &e5=QString());

void saveGeometry(const QWidget *w, const QString &section, const QString &name);
void loadGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight);

//usuwa znaki nowego wiersza, tagi htmla (wszystko co da siê dopasowaæ jako <.*>)
QString toPlainText(const QString &text);

class ChooseDescription : public QDialog
{
	Q_OBJECT
	public:
		ChooseDescription ( int nr, QWidget * parent=0, const char * name=0);
		virtual ~ChooseDescription();
		void getDescription(QString &);

	private:
		QComboBox *desc;
		QLabel *l_yetlen;

	private slots:
		void okbtnPressed();
		void cancelbtnPressed();
		void updateYetLen(const QString&);
};

class HttpClient : public QObject
{
	Q_OBJECT

	private:
		QSocket Socket;
		QString Host;
		QString Referer;
		QString Path;
		QByteArray Data;
		QByteArray PostData;
		int StatusCode;
		bool HeaderParsed;

		unsigned int ContentLength;
		bool ContentLengthNotFound;

		QMap<QString,QString> Cookies;

	private slots:
		void onConnected();
		void onReadyRead();
		void onConnectionClosed();

	public slots:
		void setHost(const QString &host);
		void get(const QString &path);
		void post(const QString &path,const QByteArray& data);
		void post(const QString &path,const QString& data);

	public:
		HttpClient();
		int status() const;
		const QByteArray& data() const;
		static QString encode(const QString& text);

	signals:
		void finished();
		void redirected(QString link);
		void error();
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
		void addElement(Element e);

	public:
		static void escapeText(QString& text);
		static void unescapeText(QString& text);

		/**
			Dodaje podany tekst do listy elementów jako
			tag html.
			@param text tekst do dodania
		**/
		void addTag(const QString &text);
		/**
			Dodaje podany tekst do listy elementów jako
			zwyk³y tekst.
			@param text tekst do dodania
		**/
		void addText(const QString &text);
		/**
			Wstawia podany tekst do listy elementów jako
			tag html przed innym elementem.
			@param pos indeks elementu przed ktorym wstawiamy
			@param text tekst do dodania
		**/
		void insertTag(const int pos,const QString &text);
		/**
			Wstawia podany tekst do listy elementów jako
			zwykly tekst przed innym elementem.
			@param pos indeks elementu przed ktorym wstawiamy
			@param text tekst do dodania
		**/
		void insertText(const int pos,const QString &text);
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
		QString generateHtml() const;
		/**
			Zwraca ilo¶æ elementów wchodz±cych w sk³ad
			dokumentu.
		**/
		int countElements() const;
		/**
			Sprawdza czy element o podanym indeksie jest
			tagiem html czy zwyk³ym tekstem.
		**/
		bool isTagElement(int index) const;
		/**
			Zwraca tekst elementu o podanym indeksie.
		**/
		const QString &elementText(int index) const;
		/**
			Zwraca tekst elementu o podanym indeksie.
		**/
		QString &elementText(int index);
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
		/**
			Skanuje tekst w poszukiwaniu adresów www,
			np. "www.kadu.net" i zmienia je w linki html.
		**/
		void convertUrlsToHtml();
};

/*
	nie u¿ywane - patrz opis w pliku .cpp

HtmlDocument GGMessageToHtmlDocument(const QString &msg, int formats_length, void *formats);
void HtmlDocumentToGGMessage(HtmlDocument &htmldoc, QString &msg,
	int &formats_length, void *&formats);
*/
class ImageWidget : public QWidget
{
	Q_OBJECT

	private:
		QImage Image;

	protected:
		virtual void paintEvent(QPaintEvent *e);

	public:
		ImageWidget(QWidget *parent);
		ImageWidget(QWidget *parent,const QByteArray &image);
		void setImage(const QByteArray &image);
		void setImage(const QPixmap &image);
};

class TokenDialog : public QDialog
{
	Q_OBJECT

	public:
		TokenDialog(QPixmap tokenImage, QDialog *parent = 0, const char *name = 0);
		void getValue(QString &tokenValue);

	private:
		QLineEdit *tokenedit;
};

class Themes : public QObject
{
	Q_OBJECT
	private:
		QStringList ThemesList;
		QStringList ThemesPaths;
		QStringList additional;
		QString ConfigName, Name, ActualTheme;
		QMap<QString, QString> entries;
		QStringList getSubDirs(const QString& path) const;
		QString fixFileName(const QString& path,const QString& fn) const;

	public:
		Themes(const QString& name, const QString& configname, const char *name=0);
		QStringList defaultKaduPathsWithThemes() const;
		const QStringList &themes() const;
		const QString &theme() const;
		const QStringList &paths() const;
		const QStringList &additionalPaths() const;
		QString themePath(const QString& theme="") const;
		QString getThemeEntry(const QString& name) const;
	public slots:
		void setTheme(const QString& theme);
		void setPaths(const QStringList& paths);

	signals:
		void themeChanged(const QString& theme);
		void pathsChanged(const QStringList& list);
};

class IconsManager :public Themes
{
	Q_OBJECT
	public:
		IconsManager(const QString& name, const QString& configname);
		/**
			Zwraca pe³n± ¶cie¿kê do ikony z aktualnego zestawu
			lub bezposrednio name je¶li jest to pe³na ¶cie¿ka.
			@param name nazwa ikony z zestawu lub sciezka do pliku
			(jesli zawiera znak '/' to jest interpretowana jako
			sciezka).
		**/
		QString iconPath(const QString &name) const;
		/**
			£aduje ikonê z aktualnego zestawu lub z podanego pliku.
			@param name nazwa ikony z zestawu lub ¶cie¿ka do pliku
			(je¶li zawiera znak '/' to jest interpretowana jako
			¶cie¿ka).
		**/
		const QPixmap & loadIcon(const QString &name);
		static void initModule();
		
		void registerMenu(QMenuData *menu);
		void unregisterMenu(QMenuData *menu);
		
		void registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName);
		void unregisterMenuItem(QMenuData *menu, const QString &caption);
	public slots:
		void clear();
		void refreshMenus();

	private:
		QMap<QString, QPixmap> icons;
		
		QValueList<QPair<QMenuData *, QValueList<QPair<QString, QString> > > > menus;

	private slots:
		void selectedPaths(const QStringList& paths);
		void onCreateConfigDialog();
		void onApplyConfigDialog();
};

//TODO: po wydaniu 0.4 trzeba zmieniæ nazwê na icons_manager i wywaliæ define'a
extern IconsManager *icons_manager_ptr;
#define icons_manager (*icons_manager_ptr)


/**
	klasa rozwiazujaca problem z powiadomieniem
	o utworzeniu nowej instancji danej klasy.
	umieszczamy w klasie publiczna statyczna
	zmienna createNotifier klasy CreateNotifier
	do ktorej mog± siê pod³±czaæ pozosta³e cze¶ci kodu.
	przed wyj¶ciem z konstruktora wywo³ujemy metodê:
	createNotifier.notify(this);
**/
class CreateNotifier : public QObject
{
	Q_OBJECT

	public:
		void notify(QObject* new_object);

	signals:
		void objectCreated(QObject* new_object);
};

class GaduImagesManager
{
	private:
		struct ImageToSend
		{
			uint32_t size;
			uint32_t crc32;
			QString file_name;
			char* data;
		};
		QValueList<ImageToSend> ImagesToSend;
		struct SavedImage
		{
			uint32_t size;
			uint32_t crc32;
			QString file_name;
		};
		QValueList<SavedImage> SavedImages;

	public:
		static void setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col);
		
		static QString loadingImageHtml(UinType uin,uint32_t size,uint32_t crc32);
		static QString imageHtml(const QString& file_name);
		void addImageToSend(const QString& file_name,uint32_t& size,uint32_t& crc32);
		void sendImage(UinType uin,uint32_t size,uint32_t crc32);
		/**
			Szuka zakolejkowanego obrazka i zwraca jego nazwê pliku
			Zwraca ci±g pusty, je¶li obrazek nie zosta³ w tej sesji
			zakolejkowany do wys³ania.
		**/
		QString getImageToSendFileName(uint32_t size,uint32_t crc32);
		/**
			Zapisuje obrazek w katalogu .gg/images.
			Zwraca pe³n± ¶cie¿kê do zapisanego obrazka.
		**/
		QString saveImage(UinType sender,uint32_t size,uint32_t crc32,const QString& filename,const char* data);
		/**
			Szuka zapisanego obrazka i zwraca jego nazwê pliku
			wraz ze ¶cie¿k±. Zwraca ci±g pusty, je¶li obrazek
			nie zosta³ w tej sesji zapisany.
		**/
		QString getSavedImageFileName(uint32_t size,uint32_t crc32);
		QString replaceLoadingImages(const QString& text,UinType sender,uint32_t size,uint32_t crc32);
};

extern GaduImagesManager gadu_images_manager;

class PixmapPreview : public QLabel, public QFilePreview
{
	public:
		PixmapPreview();
		void previewUrl(const QUrl& url);
};

class ImageDialog : public QFileDialog
{
	public:
		ImageDialog(QWidget* parent);
};

/**
	Zmodyfikowany QTextBrowser specjalnie na potrzeby Kadu.
	Klikniêcie na linku otwiera ustawion± w konfiguracji przegl±darkê.
	W menu kontekstowym jest dodatkowa opcja "Kopiuj lokacjê odno¶nika".
	Dodatkowo poprawka b³êdu w Qt.
**/
class KaduTextBrowser : public QTextBrowser, QToolTip
{
	Q_OBJECT

	private:
		QString anchor;
		int level;
		/**
			this value stores the current highlighted link
			for use with maybeTip(), or is null
		**/
		QString highlightedlink; 	

	private slots:
		void copyLinkLocation();
		void hyperlinkClicked(const QString& link) const;
		void linkHighlighted(const QString &);
		
	protected:
		QPopupMenu *createPopupMenu(const QPoint &point);
		virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph);
		virtual void maybeTip(const QPoint&);
		virtual void contentsMouseReleaseEvent(QMouseEvent * e);

	public:
		KaduTextBrowser(QWidget *parent = 0, const char *name = 0);
		void setSource(const QString &name);
		/**
			Nadpisane dla wyja¶nienia wieloznaczno¶ci
		**/
		void clear() 			{ QTextBrowser::clear(); }
		/**
			Nadpisane dla wyja¶nienia wieloznaczno¶ci		**/

		void setFont(const QFont& f) 	{ QTextBrowser::setFont(f); }
		void setMargin(int width);

	public slots:
		virtual void copy();

	signals:
		/**
			Dowolny przycisk myszy zosta³ zwolniony
			Przekazany zostaje tak¿e obiekt, który wywo³a³ akcjê - czyli this.
		**/
		void mouseReleased(QMouseEvent *e, KaduTextBrowser *sender);
};

QValueList<int> toIntList(const QValueList<QVariant> &in);
QValueList<QVariant> toVariantList(const QValueList<int> &in);

template<class T, class X> QValueList<T> keys(const QMap<T, X> &m)
{
#if QT_VERSION < 0x030005
    QValueList<T> ret;
    for(QMap<T,X>::const_iterator it=m.begin(); it!=m.end(); it++)
        ret.append(it.key());
    return ret;
#else
    return m.keys();
#endif
}

template<class T, class X> QValueList<X> values(const QMap<T, X> &m)
{
#if QT_VERSION < 0x030005
    QValueList<X> ret;
    for(QMap<T,X>::const_iterator it=m.begin(); it!=m.end(); it++)
        ret.append(it.data());
    return ret;
#else
    return m.values();
#endif
}

/*
	zastêpstwo dla arga w QString, które podmienia kolejne %[1-4] w miejscu
	
	w QStringu efektem:
		QString("%1 odstêp %2").arg("pierwszy %1 tekst").arg("drugi tekst") jest "pierwszy drugi tekst tekst odstêp %2"
	a chcieliby¶my ¿eby by³o
		"pierwszy %1 tekst odstêp drugi tekst"
	co robi w³a¶nie ta funkcja
*/
QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3=QString(), const QString &arg4=QString());

QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3, const QString &arg4,
				const QString &arg5, const QString &arg6=QString(),
				const QString &arg7=QString(),const QString &arg8=QString(),
				const QString &arg9=QString());

/**
	uogólniony narg(const QString&, const QString &, const QString &, const QString &, const QString &)
	na wiêksz± liczbê parametrów
	count musi byæ <=9
	tab - tablica count wska¼ników do QString
**/
QString narg(const QString &s, const QString **tab, int count);

void printBacktrace(const QString &header="");

// definiuje now± zmienn± v o takim samym typie jak x jednocze¶nie inicjuj±c j± warto¶ci± x
#define VAR(v,x)	__typeof(x) v=x

// szybsza w zapisaniu pêtla for
#define FOREACH(i,c) for(VAR(i, (c).begin()); i!=(c).end(); ++i)

#endif
