#ifndef KADU_MISC_H
#define KADU_MISC_H

#include <qdatetime.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qimage.h>
#include <qlabel.h>
#include <qstring.h>

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
QString dataPath(const QString &f = "", const char *argv0 = 0);

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
QString pwHash(const QString &text);
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);
void openWebBrowser(const QString &link);
QString parse(const QString &s, const UserListElement &ule, bool escape = true);
void stringHeapSort(QStringList &c);
QStringList toStringList(const QString &e1, const QString &e2=QString(), const QString &e3=QString(), const QString &e4=QString(), const QString &e5=QString());

void saveGeometry(const QWidget *w, const QString &section, const QString &name);
void loadGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight);

//usuwa znaki nowego wiersza, tagi htmla (wszystko co da siê dopasowaæ jako <.*>)
QString toPlainText(const QString &text);

extern QFont *defaultFont;
extern QFontInfo *defaultFontInfo;

extern QTextCodec *codec_cp1250;
extern QTextCodec *codec_latin2;

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


QValueList<int> toIntList(const QValueList<QVariant> &in);
QValueList<QVariant> toVariantList(const QValueList<int> &in);

template<class T, class X> QValueList<T> keys(const QMap<T, X> &m)
{
#if QT_VERSION < 0x030005
    QValueList<T> ret;
    for(QMap<T,X>::const_iterator it = m.begin(), map_end = m.end(); it != map_end; ++it)
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
    for(QMap<T,X>::const_iterator it = m.begin(), map_end = m.end(); it != map_end; ++it)
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

#if QT_VERSION >= 0x030200
#define CONST_FOREACH(i,c) for(VAR(i, (c).constBegin()), _kadu__##i##__end = (c).constEnd(); i!=_kadu__##i##__end; ++i)
#else
#define CONST_FOREACH(i,c) for(VAR(i, (c).begin()), _kadu__##i##__end = (c).end(); i!=_kadu__##i##__end; ++i)
#endif

#endif
