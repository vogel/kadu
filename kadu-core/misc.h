#ifndef KADU_MISC_H
#define KADU_MISC_H

// TODO remove
#include <Qt3Support/Q3FileDialog>

#include "status.h"

class QComboBox;
class QLabel;
class QPushButton;

/*
	Zmienia �cie�k� relatywn� do katalogu z ustawieniami gg
	na �cie�k� bezwzgledn� uwzgl�dniaj�c zmienne �rodowiskowe
	$HOME i $CONFIG_DIR
*/
QString ggPath(const QString &subpath = QString::null);

/*
	zwraca �cie�k� do pliku f
	je�eli drugi parametr nie jest == 0, to funkcja pr�buje najpierw ustali�
	�cie�k� na podstawie argv0, kt�re ma by� r�wne argv[0] oraz zmiennej PATH
*/
QString dataPath(const QString &f = QString::null, const char *argv0 = 0);

QString libPath(const QString &f = QString::null);

QString cp2unicode(const QByteArray &);
QByteArray unicode2cp(const QString &);
QString latin2unicode(const QByteArray &);
QByteArray unicode2latin(const QString &);
QString unicode2std(const QString &);

// TODO: why not use nromal QUrl::encode ?
//zamienia kodowanie polskich znak�w przekonwertowane z utf-8 przy pomocy QUrl::encode na kodowanie latin-2
QString unicodeUrl2latinUrl(const QString &buf);
//zamienia polskie znaki na format latin2 "url" (czyli do postaci %XY)
QString unicode2latinUrl(const QString &buf);

QString printDateTime(const QDateTime &datetime);
QString timestamp(time_t = 0);
QDateTime currentDateTime();
QString pwHash(const QString &text);
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);

void openWebBrowser(const QString &link);
void openMailClient(const QString &mail);
void openGGChat(const QString &gg);

QString versionToName(const unsigned int version);

void stringHeapSort(QStringList &c);
QStringList toStringList(const QString &e1, const QString &e2=QString(), const QString &e3=QString(), const QString &e4=QString(), const QString &e5=QString());

void saveGeometry(const QWidget *w, const QString &section, const QString &name);
void loadGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight);

//usuwa znaki nowego wiersza, tagi htmla (wszystko co da si� dopasowa� jako <.*>)
QString toPlainText(const QString &text);

extern QFont *defaultFont;
extern QFontInfo *defaultFontInfo;

extern QTextCodec *codec_cp1250;
extern QTextCodec *codec_latin2;

class ChooseDescription : public QDialog
{
	Q_OBJECT

	static ChooseDescription *Dialog;

	QPushButton *OkButton;
	QComboBox *Description;
	QLabel *AvailableChars;

	UserStatus Status;

private slots:
	void okPressed();
	void cancelPressed();
	void updateAvailableChars(const QString &);

public:
	static void show(const UserStatus &status, const QPoint &position);

	ChooseDescription(QWidget *parent = 0);
	virtual ~ChooseDescription();

	void setStatus(const UserStatus &status);
	void setPosition(const QPoint &position);

};

class OpenChatWith : public QWidget
{
	Q_OBJECT

	QComboBox *c_text;
	QComboBox *c_protocol;

private slots:
	void inputAccepted();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	OpenChatWith(QWidget *parent = 0);
	virtual ~OpenChatWith();

};

class ImageWidget : public QWidget
{
	QImage Image;

protected:
	virtual void paintEvent(QPaintEvent *e);

public:
	ImageWidget(QWidget *parent);
	ImageWidget(const QByteArray &image, QWidget *parent);
	void setImage(const QByteArray &image);
	void setImage(const QPixmap &image);

};

class TokenDialog : public QDialog
{
	Q_OBJECT

	QLineEdit *tokenedit;

public:
	TokenDialog(QPixmap tokenImage, QDialog *parent = 0);
	void getValue(QString &tokenValue);

};

/**
	klasa rozwiazujaca problem z powiadomieniem
	o utworzeniu nowej instancji danej klasy.
	umieszczamy w klasie publiczna statyczna
	zmienna createNotifier klasy CreateNotifier
	do ktorej mog� si� pod��cza� pozosta�e cze�ci kodu.
	przed wyj�ciem z konstruktora wywo�ujemy metod�:
	createNotifier.notify(this);
**/
class CreateNotifier : public QObject
{
	Q_OBJECT

public:
	void notify(QObject *new_object);

signals:
	void objectCreated(QObject *new_object);

};

/*
class PixmapPreview : public QLabel, public Q3FilePreview
{
	public:
		PixmapPreview();
		void previewUrl(const Q3Url& url);
};*/

class ImageDialog : public Q3FileDialog
{
public:
	ImageDialog(QWidget *parent);

};

class QLabel;
class Q3SimpleRichText;

QList<int> toIntList(const QList<QVariant> &in);
QList<QVariant> toVariantList(const QList<int> &in);

/*
	zast�pstwo dla arga w QString, kt�re podmienia kolejne %[1-4] w miejscu

	w QStringu efektem:
		QString("%1 odst�p %2").arg("pierwszy %1 tekst").arg("drugi tekst") jest "pierwszy drugi tekst tekst odst�p %2"
	a chcieliby�my �eby by�o
		"pierwszy %1 tekst odst�p drugi tekst"
	co robi w�a�nie ta funkcja
*/
QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3=QString(), const QString &arg4=QString());

QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3, const QString &arg4,
				const QString &arg5, const QString &arg6=QString(),
				const QString &arg7=QString(),const QString &arg8=QString(),
				const QString &arg9=QString());

/**
	uog�lniony narg(const QString&, const QString &, const QString &, const QString &, const QString &)
	na wi�ksz� liczb� parametr�w
	count musi by� <=9
	tab - tablica count wska�nik�w do QString
**/
QString narg(const QString &s, const QString **tab, int count);

void printBacktrace(const QString &header = QString::null);

// private
extern long long int startTime, beforeExecTime, endingTime, exitingTime;
extern bool measureTime;

extern const char *detailed_version;

#endif
