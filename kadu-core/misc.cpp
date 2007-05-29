/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qprocess.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qsimplerichtext.h>
#include <qtextcodec.h>

//getpwuid
#include <pwd.h>

#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "html_document.h"
#include "icons_manager.h"
#include "kadu-config.h"
#include "kadu_parser.h"
#include "message_box.h"
#include "misc.h"
#include "userlistelement.h"

#define GG_FONT_IMAGE	0x80

QFont *defaultFont;
QFontInfo *defaultFontInfo;

QTextCodec *codec_cp1250 = QTextCodec::codecForName("CP1250");
QTextCodec *codec_latin2 = QTextCodec::codecForName("ISO8859-2");

long long int startTime = 0, beforeExecTime = 0, endingTime = 0, exitingTime = 0;
bool measureTime = false;

void saveGeometry(const QWidget *w, const QString &section, const QString &name)
{
	QRect geom;
	geom.setX(w->pos().x());
	geom.setY(w->pos().y());
	geom.setWidth(w->size().width());
	geom.setHeight(w->size().height());

	config_file.writeEntry(section, name, geom);
}

void loadGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight)
{
	QRect def_rect(defaultX, defaultY, defaultWidth, defaultHeight);
	config_file.addVariable(section, name, def_rect);

	QRect geom=config_file.readRectEntry(section, name);
	w->resize(geom.width(),geom.height());
	w->move(geom.x(),geom.y());
}

QString ggPath(const QString &subpath)
{
	static QString path=QString::null;
	if (path==QString::null)
	{
		char *home;
		struct passwd *pw;
		if ((pw = getpwuid(getuid())))
			home = pw->pw_dir;
		else
			home = getenv("HOME");
		KaduParser::globalVariables["HOME"] = home;
		char *config_dir = getenv("CONFIG_DIR");
#ifdef Q_OS_MACX
		if (config_dir == NULL)
			path = QString("%1/Library/Kadu/").arg(home);
		else
			path = QString("%1/%2/Kadu/").arg(home).arg(config_dir);
#else
		if (config_dir == NULL)
			path = QString("%1/.kadu/").arg(home);
		else
			path = QString("%1/%2/kadu/").arg(home).arg(config_dir);
#endif
		KaduParser::globalVariables["KADU_CONFIG"] = path;
	}
	return path+subpath;
}

//stat,getcwd
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//getenv
#include <stdlib.h>
//memcpy,strcat,strchr,strerror
#include <string.h>

#include <errno.h>
/*
	sprawdza czy wskazana ¶cie¿ka jest linkiem symbolicznym	i je¿eli jest,
	to próbuje wyci±gn±æ ¶cie¿kê na któr± wskazuje ten link
	zwraca b³±d tylko gdy wyst±pi jaki¶ b³±d przy wywo³ywaniu readlink (co wskazuje na jaki¶ powa¿ny b³±d)
	uwaga: je¿eli pliku nie ma, to funkcja zwraca _sukces_, bo plik nie jest linkiem
 */
static bool delinkify(char *path, int maxlen)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", path);
	struct stat st;
	if (lstat(path, &st) == -1)
	{
		kdebugf2();
		return true;
	}
	kdebugm(KDEBUG_INFO, "mode: %o\n", st.st_mode);
	if (!S_ISLNK(st.st_mode))
	{
		kdebugf2();
		return true;
	}
	char *path2 = new char[maxlen];
	ssize_t bytesFilled = readlink(path, path2, maxlen - 1);
	if (bytesFilled == -1)
	{
		fprintf(stderr, "readlink error: '%s'\n", strerror(errno));
		fflush(stderr);
		delete [] path2;
		kdebugf2();
		return false;
	}
	path2[bytesFilled] = 0;
	memcpy(path, path2, bytesFilled + 1);
	delete [] path2;
	kdebugf2();
	return true;
}

/*
	funkcja poszukuje binarki programu na podstawie argv[0] oraz zmiennej PATH
	je¿eli j± znajdzie, to zapisuje ¶cie¿kê pod adres wskazany przez path
	(o maksymalnej d³ugo¶ci len) oraz zwraca path, który zakañczany jest znakiem '/'
	je¿eli binarka nie zostanie znaleziona, to zwracany jest NULL
	w obu przypadkach gwarantowane jest, ¿e path koñczy siê znakiem 0
	(len musi byæ > 2)
*/
static char *findMe(const char *argv0, char *path, int len)
{
	kdebugf();
	struct stat buf;
	char *lastslash;

	char *current;
	char *previous;
	int l;


	if (argv0[0] == '.' && argv0[1] == '/') //¶cie¿ka wzglêdem bie¿±cego katalogu (./)
	{
		if (getcwd(path, len - 2) == NULL)
		{
			path[0] = 0;
			kdebugf2();
			return NULL;
		}
		strncat(path, argv0 + 1, len - 1);
		path[len - 1] = 0;
		if (!delinkify(path, len))
		{
			kdebugf2();
			return NULL;
		}
		lastslash = strrchr(path, '/');
		lastslash[1] = 0;
		kdebugf2();
		return path;
	}

	if (argv0[0] == '.' && argv0[1] == '.' && argv0[2] == '/') //¶cie¿ka wzglêdem bie¿±cego katalogu (../)
	{
		if (getcwd(path, len - 2)==NULL)
		{
			path[0] = 0;
			kdebugf2();
			return NULL;
		}
		strncat(path, "/", len - 1);
		strncat(path, argv0, len - 1);
		path[len - 1] = 0;
		if (!delinkify(path, len))
		{
			kdebugf2();
			return NULL;
		}
		lastslash = strrchr(path, '/');
		lastslash[1] = 0;
		kdebugf2();
		return path;
	}

	if (argv0[0] == '/') //¶cie¿ka bezwzglêdna
	{
		strncpy(path, argv0, len - 1);
		path[len - 1] = 0;
		if (!delinkify(path, len))
		{
			kdebugf2();
			return NULL;
		}
		lastslash = strrchr(path, '/');
		lastslash[1] = 0;
		kdebugf2();
		return path;
	}

	previous = getenv("PATH"); //szukamy we wszystkich katalogach, które s± w PATH
	while((current = strchr(previous, ':')))
	{
		l = current - previous;
		if (l > len - 2)
		{
			path[0] = 0;
			kdebugf2();
			return NULL;
		}

		memcpy(path, previous, l);
		path[l] = '/';
		path[l + 1] = 0;
		strncat(path, argv0, len);
		path[len - 1] = 0;
		if (!delinkify(path, len))
		{
			kdebugf2();
			return NULL;
		}
		if (stat(path, &buf) != -1)
		{
			if (path[l - 1] == '/')
				path[l] = 0;
			else
				path[l + 1] = 0;
			kdebugf2();
			return path;
		}
		previous = current + 1;
	}
	//nie znale¼li¶my dot±d (bo szukali¶my ':'), wiêc mo¿e w pozosta³ej czê¶ci co¶ siê znajdzie?
	strncpy(path, previous, len - 2);
	path[len - 2] = 0;

	l = strlen(path);
	path[l] = '/';
	path[l + 1] = 0;
	strncat(path, argv0, len);
	path[len - 1] = 0;
	if (!delinkify(path, len))
	{
		kdebugf2();
		return NULL;
	}
	if (stat(path, &buf) != -1)
	{
		if (path[l - 1] == '/')
			path[l] = 0;
		else
			path[l + 1] = 0;
		kdebugf2();
		return path;
	}
	else
	{
		path[0] = 0;
		kdebugf2();
		return NULL;
	}
}

static QString lib_path;
static QString data_path;

QString libPath(const QString &f)
{
	return lib_path + f;
}

QString dataPath(const QString &p, const char *argv0)
{
	if (argv0 != 0)
	{
#ifdef Q_OS_MACX
		char cpath[1024];
		if (findMe(argv0, cpath, 1024)==NULL)
		{
			fprintf(stderr, "we've got real problem here ;)\n");
			fflush(stderr);
			exit(10);
		}
		else
		{
			data_path = QString(cpath) + "../../";
			lib_path = QString(cpath) + "../../";
		}
#else
		QString datadir(DATADIR);
		QString bindir(BINDIR);
		QString libdir(LIBDIR);

		//je¿eli ¶cie¿ki nie koñcz± siê na /share i /bin oraz gdy bez tych koñcówek
		//¶cie¿ki siê nie pokrywaj±, to znaczy ¿e kto¶ ustawi³ rêcznie DATADIR lub BINDIR
		if (!datadir.endsWith("/share") || !bindir.endsWith("/bin") || !libdir.endsWith("/lib") ||
			datadir.left(datadir.length() - 6) != bindir.left(bindir.length() - 4) ||
			bindir.left(bindir.length() - 4) != libdir.left(libdir.length() - 4))
		{
			data_path = datadir + '/';
			lib_path = libdir + '/';
		}
		else
		{
			char cpath[1024];
			if (findMe(argv0, cpath, 1024) == NULL)
			{
				data_path = datadir + '/';
				lib_path = libdir + '/';
			}
			else
			{
				data_path = QString(cpath) + "../share/";
				lib_path = QString(cpath) + "../lib/";
			}
		}
#endif
		KaduParser::globalVariables["DATA_PATH"] = data_path;
		KaduParser::globalVariables["LIB_PATH"] = lib_path;
	}
	if (data_path.isEmpty())
	{
		kdebugm(KDEBUG_PANIC, "dataPath() called _BEFORE_ initial dataPath(\"\",argv[0]) (static object uses dataPath()?) !!!\n");
		printBacktrace("dataPath(): constructor of static object uses dataPath");
	}
	kdebugm(KDEBUG_INFO, "%s%s\n", data_path.local8Bit().data(), p.local8Bit().data());
	return data_path + p;
}

QString cp2unicode(const unsigned char *buf)
{
	if (buf)
		return codec_cp1250->toUnicode((const char*)buf);
	else
		return QString::null;
}

QString unicode2cp(const QString &buf)
{
	return codec_cp1250->fromUnicode(buf);
}

QString latin2unicode(const unsigned char *buf)
{
	if (buf)
		return codec_latin2->toUnicode((const char*)buf);
	else
		return QString::null;
}

QCString unicode2latin(const QString &buf)
{
	return codec_latin2->fromUnicode(buf);
}

QString unicode2std(const QString &buf)
{
	QString tmp = buf;
	tmp.replace(QChar(0x0119), 'e');
	tmp.replace(QChar(0x00f3), 'o');
	tmp.replace(QChar(0x0105), 'a');
	tmp.replace(QChar(0x015b), 's');
	tmp.replace(QChar(0x0142), 'l');
	tmp.replace(QChar(0x017c), 'z');
	tmp.replace(QChar(0x017a), 'z');
	tmp.replace(QChar(0x0107), 'c');
	tmp.replace(QChar(0x0144), 'n');
	tmp.replace(QChar(0x0118), 'E');
	tmp.replace(QChar(0x00d3), 'O');
	tmp.replace(QChar(0x0104), 'A');
	tmp.replace(QChar(0x015a), 'S');
	tmp.replace(QChar(0x0141), 'L');
	tmp.replace(QChar(0x017b), 'Z');
	tmp.replace(QChar(0x0179), 'Z');
	tmp.replace(QChar(0x0106), 'C');
	tmp.replace(QChar(0x0143), 'N');
	return tmp;
}

QString unicode2latinUrl(const QString &buf)
{
	QString tmp = buf;
	tmp.replace(QChar(0x0119), "%EA");
	tmp.replace(QChar(0x00f3), "%F3");
	tmp.replace(QChar(0x0105), "%B1");
	tmp.replace(QChar(0x015b), "%B6");
	tmp.replace(QChar(0x0142), "%B3");
	tmp.replace(QChar(0x017c), "%BF");
	tmp.replace(QChar(0x017a), "%BC");
	tmp.replace(QChar(0x0107), "%E6");
	tmp.replace(QChar(0x0144), "%F1");
	tmp.replace(QChar(0x0118), "%CA");
	tmp.replace(QChar(0x00d3), "%D3");
	tmp.replace(QChar(0x0104), "%A1");
	tmp.replace(QChar(0x015a), "%A6");
	tmp.replace(QChar(0x0141), "%A3");
	tmp.replace(QChar(0x017b), "%AF");
	tmp.replace(QChar(0x0179), "%AC");
	tmp.replace(QChar(0x0106), "%C3");
	tmp.replace(QChar(0x0143), "%D1");
	return tmp;
}

//looks magically, does it? :D
QString unicodeUrl2latinUrl(const QString &buf)
{
	QString tmp = buf;
	tmp.replace("%C4%99", "%EA"); //ê
	tmp.replace("%C3%B3", "%F3"); //ó
	tmp.replace("%C4%85", "%B1"); //±
	tmp.replace("%C5%9B", "%B6"); //¶
	tmp.replace("%C5%82", "%B3"); //³
	tmp.replace("%C5%BC", "%BF"); //¿
	tmp.replace("%C5%BA", "%BC"); //¼
	tmp.replace("%C4%87", "%E6"); //æ
	tmp.replace("%C5%84", "%F1"); //ñ
	tmp.replace("%C4%98", "%CA"); //Ê
	tmp.replace("%C3%93", "%D3"); //Ó
	tmp.replace("%C4%84", "%A1"); //¡
	tmp.replace("%C5%9A", "%A6"); //¦
	tmp.replace("%C5%81", "%A3"); //£
	tmp.replace("%C5%BB", "%AF"); //¯
	tmp.replace("%C5%B9", "%AC"); //¬
	tmp.replace("%C4%86", "%C3"); //Æ
	tmp.replace("%C5%83", "%D1"); //Ñ
	return tmp;
}

QString printDateTime(const QDateTime &datetime)
{
	QString tmp;
	time_t t;
	QDateTime dt2;
	int delta;

	t = time(NULL);
	dt2.setTime_t(t);
	dt2.setTime(QTime(0, 0));
	tmp = datetime.toString("hh:mm:ss");
	delta = dt2.secsTo(datetime);
//	kdebugmf(KDEBUG_INFO, "%d\n", delta);
	if (delta < 0 || delta >= 3600 * 24)
		tmp.append(datetime.toString(" (dd.MM.yyyy)"));
	return tmp;
}

QString timestamp(time_t customtime)
{
	QString buf;
	QDateTime date;
	time_t t;

	t = time(NULL);

	date.setTime_t(t);
	buf.append(printDateTime(date));

	if (customtime)
	{
		date.setTime_t(customtime);
		buf.append(QString(" / S ") + printDateTime(date));
	}

	return buf;
}

QDateTime currentDateTime(void)
{
	time_t t;
	QDateTime date;

	t = time(NULL);
	date.setTime_t(t);
	return date;
}

QStringList toStringList(const QString &e1, const QString &e2, const QString &e3, const QString &e4, const QString &e5)
{
	QStringList list(e1);
	if (e2!=QString::null)
		list<<e2;
	if (e3!=QString::null)
		list<<e3;
	if (e4!=QString::null)
		list<<e4;
	if (e5!=QString::null)
		list<<e5;
	return list;
}

QString pwHash(const QString &text)
{
	QString newText = text;
	for (unsigned int i = 0, textLength = text.length(); i < textLength; ++i)
		newText[i] = QChar(text[i].unicode() ^ i ^ 1);
	return newText;
}

QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n)
{
	static const int langSize = 5;
	static const char local[][3] = {"en",
		"de",
		"fr",
		"it",
		"pl"};

	static const char name[][sizeof("English") /*length of the longest*/] = {
		QT_TR_NOOP("English"),
		QT_TR_NOOP("German"),
		QT_TR_NOOP("French"),
		QT_TR_NOOP("Italian"),
		QT_TR_NOOP("Polish")};

	for (int i = 0; i < langSize; ++i)
	{
		if (l2n)
		{
			if (locale.mid(0, 2) == local[i])
				return application->translate("@default", name[i]);
		}
		else
			if (locale == application->translate("@default", name[i]))
				return local[i];
	}
	if (l2n)
		return application->translate("@default", QT_TR_NOOP("English"));
	else
		return "en";
}

void openWebBrowser(const QString &link)
{
	kdebugf();

	QString webBrowser = config_file.readEntry("Chat","WebBrowser");
	if (webBrowser.isEmpty())
	{
		MessageBox::msg(qApp->translate("@default", QT_TR_NOOP("Web browser was not specified. Visit the configuration section")), false, "Warning");
		kdebugmf(KDEBUG_INFO, "Web browser NOT specified.\n");
		return;
	}
	if (!webBrowser.contains("%1"))
		webBrowser.append(" \"" + unicode2latinUrl(link) + '"');
	else
		webBrowser.replace("%1", unicode2latinUrl(link));

	QStringList args = toStringList("sh", "-c", webBrowser);

	CONST_FOREACH(i, args)
		kdebugmf(KDEBUG_INFO, "%s\n", (*i).local8Bit().data());

	QProcess *browser = new QProcess(args, qApp);
	QObject::connect(browser, SIGNAL(processExited()), browser, SLOT(deleteLater()));

	if (!browser->start())
		MessageBox::msg(qApp->translate("@default", QT_TR_NOOP("Could not spawn Web browser process. Check if the Web browser is functional")), false, "Critical");

	kdebugf2();
}

void openMailClient(const QString &mail)
{
	kdebugf();

	QString mailClient = config_file.readEntry("Chat", "MailClient", QString::null);
	if (mailClient.isEmpty())
	{
		MessageBox::msg(qApp->translate("@default", QT_TR_NOOP("Mail client was not specified. Visit the configuration section")), false, "Warning");
		kdebugmf(KDEBUG_INFO, "Mail client NOT specified.\n");
		return;
	}

	QString email = mail;

	if (email.startsWith("mailto:"))
		email.remove(0, 7); // usuwamy "mailto:", je¶li zosta³o dodane jako fragment adresu

	if (mailClient.contains("%1"))
		mailClient.replace("%1", email);
	else
		mailClient.append(email);

	QStringList args = toStringList("sh", "-c", mailClient);

	CONST_FOREACH(arg, args)
		kdebugmf(KDEBUG_INFO, "%s\n", (*arg).local8Bit().data());

	QProcess *mailer = new QProcess(args, qApp);
	QObject::connect(mailer, SIGNAL(processExited()), mailer, SLOT(deleteLater()));

	if (!mailer->start())
		MessageBox::msg(qApp->translate("@default", QT_TR_NOOP("Could not spawn Mail client process. Check if the Mail client is functional")), false, "Critical");

	kdebugf2();
}

QString versionToName(const unsigned int version)
{
	kdebugf();
	QString name;

	switch (version)
	{
		case 0x20: name = "GG 6.0b129"; break;
		case 0x21: name = "GG 6.0b133"; break;
		case 0x22: name = "GG 6.0b140"; break;
		case 0x24: name = "GG 6.1b155/7.6b1359"; break;
		case 0x25: name = "GG 7.0b1"; break;
		case 0x26: name = "GG 7.0b20"; break;
		case 0x27: name = "GG 7.0b22"; break;
		case 0x28: name = "GG 7.5b2201"; break;
		case 0x29: name = "GG 7.6b1688"; break;
		case 0x2a: name = "GG 7.7b3315"; break;
		default: name = "Unknown"; break;
	}

	return name;
	kdebugf2();
}

//internal usage
static void stringHeapSortPushDown( QString* heap, int first, int last )
{
	int r = first;
	while ( r <= last / 2 ) {
		if ( last == 2 * r ) {
			if ( heap[2 * r].localeAwareCompare(heap[r])<0 )
				qSwap( heap[r], heap[2 * r] );
			r = last;
		} else {
			if ( heap[2 * r].localeAwareCompare( heap[r] )<0 && !(heap[2 * r + 1].localeAwareCompare(heap[2 * r])<0) ) {
				qSwap( heap[r], heap[2 * r] );
				r *= 2;
			} else if ( heap[2 * r + 1].localeAwareCompare( heap[r] )<0 && heap[2 * r + 1].localeAwareCompare( heap[2 * r] )<0 ) {
				qSwap( heap[r], heap[2 * r + 1] );
				r = 2 * r + 1;
			} else {
				r = last;
			}
		}
    }
}

//internal usage
static void stringHeapSortHelper( QStringList::iterator b, QStringList::iterator e, QString, uint n )
{
	QStringList::iterator insert = b;
	QString* realheap = new QString[n];
	QString* heap = realheap - 1;
	int size = 0;
	for( ; insert != e; ++insert ) {
		heap[++size] = *insert;
		int i = size;
		while( i > 1 && heap[i].localeAwareCompare(heap[i / 2])<0 ) {
			qSwap( heap[i], heap[i / 2] );
			i /= 2;
		}
	}

	for( uint i = n; i > 0; --i ) {
		*b++ = heap[1];
		if ( i > 1 ) {
			heap[1] = heap[i];
			stringHeapSortPushDown( heap, 1, (int)i - 1 );
		}
	}

	delete[] realheap;
}

void stringHeapSort(QStringList &c)
{
	if (c.begin() == c.end())
		return;
	stringHeapSortHelper(c.begin(), c.end(), *(c.begin()), (uint)c.count());
}

ChooseDescription::ChooseDescription(int nr, QPoint *position, QWidget * parent, const char * name)
	: QDialog(parent, name, false), desc(0), l_yetlen(0)
{
	kdebugf();
	setCaption(tr("Select description"));

	while (defaultdescriptions.count()>config_file.readUnsignedNumEntry("General", "NumberOfDescriptions"))
		defaultdescriptions.pop_back();

  	desc = new QComboBox(TRUE, this, "description");
	desc->setSizeLimit(30);
	desc->insertStringList(defaultdescriptions);

	QLineEdit *ss = new QLineEdit(this, "LineEdit");
	desc->setLineEdit(ss);

	l_yetlen = new QLabel(this);
	updateYetLen(desc->currentText());

	connect(desc, SIGNAL(textChanged(const QString&)), this, SLOT(updateYetLen(const QString&)));

	QPixmap pix;
	const char *iconName;
	switch (nr)
	{
		case 1: iconName = "OnlineWithDescription";		break;
		case 3: iconName = "BusyWithDescription";		break;
		case 5: iconName = "InvisibleWithDescription";	break;
		case 7:
		default:iconName = "OfflineWithDescription";
	}
	pix = icons_manager->loadIcon(iconName);

	QPushButton *okbtn = new QPushButton(QIconSet(pix), tr("&OK"), this);
	QPushButton *cancelbtn = new QPushButton(tr("&Cancel"), this);

	QObject::connect(okbtn, SIGNAL(clicked()), this, SLOT(okbtnPressed()));
	QObject::connect(cancelbtn, SIGNAL(clicked()), this, SLOT(cancelbtnPressed()));

	QGridLayout *grid = new QGridLayout(this, 2, 2, 5, 10);

	grid->addMultiCellWidget(desc, 0, 0, 0, 2);
	grid->addWidget(l_yetlen, 1, 0);
	grid->addWidget(okbtn, 1, 1, Qt::AlignRight);
	grid->addWidget(cancelbtn, 1, 2, Qt::AlignRight);
	grid->addColSpacing(0, 200);

	int width = 250;
	int height = 80;
	QSize sh = sizeHint();
	if (sh.width() > width)
		width = sh.width();
	if (sh.height() > height)
		height = sh.height();
	if (position)
	{
		QPoint p = *position;
		QDesktopWidget *d = QApplication::desktop();
		if (p.x() + width + 20 >= d->width())
			p.setX(d->width() - width - 20);
		if (p.y() + height + 20 >= d->height())
			p.setY(d->height() - height - 20);
		move(p);
	}
	resize(width, height);
	kdebugf2();
}

ChooseDescription::~ChooseDescription()
{
}

void ChooseDescription::getDescription(QString &dest)
{
	dest = desc->currentText();
}

void ChooseDescription::okbtnPressed()
{
	//je¿eli ju¿ by³ taki opis, to go usuwamy
	defaultdescriptions.remove(desc->currentText());
	//i dodajemy na pocz±tek
	defaultdescriptions.prepend(desc->currentText());

	while (defaultdescriptions.count()>config_file.readUnsignedNumEntry("General", "NumberOfDescriptions"))
		defaultdescriptions.pop_back();
	accept();
}

void ChooseDescription::cancelbtnPressed()
{
	reject();
//	close();
}

void ChooseDescription::updateYetLen(const QString& text)
{
	int length = text.length();
	int count = (length - 10) / (GG_STATUS_DESCR_MAXSIZE - 10);
	int rest = (count + 1) * (GG_STATUS_DESCR_MAXSIZE - 10) - length + 10;

	l_yetlen->setText(' ' + QString::number(rest) + " (" + QString::number(count) + ")");
}

ImageWidget::ImageWidget(QWidget *parent)
	: QWidget(parent, "ImageWidget"), Image()
{
}

ImageWidget::ImageWidget(QWidget *parent,const QByteArray &image)
	: QWidget(parent, "ImageWidget"), Image(image)
{
	setMinimumSize(Image.width(), Image.height());
}

void ImageWidget::setImage(const QByteArray &image)
{
	Image.loadFromData(image);
	setMinimumSize(Image.width(), Image.height());
}

void ImageWidget::setImage(const QPixmap &image)
{
	Image = image;
	setMinimumSize(Image.width(), Image.height());
}

void ImageWidget::paintEvent(QPaintEvent *)
{
	if (!Image.isNull())
	{
		QPainter p(this);
		p.drawImage(0,0,Image);
	}
}

// -----------------------
//      TokenDialog
// -----------------------

TokenDialog::TokenDialog(QPixmap tokenImage, QDialog *parent, const char *name)
	: QDialog(parent, name), tokenedit(0)
{
	kdebugf();
	QGridLayout *grid = new QGridLayout(this, 3, 2, 6, 5);

	QLabel *l_tokenimage = new QLabel(tr("Read this code ..."), this);
	ImageWidget *tokenimage = new ImageWidget(this);

	QLabel *l_tokenedit = new QLabel(tr("and type here"), this);
	tokenedit = new QLineEdit(this);

	QPushButton *b_ok = new QPushButton(tr("&OK"), this);
	connect(b_ok, SIGNAL(clicked()), this, SLOT(accept()));
	QPushButton *b_cancel = new QPushButton(tr("&Cancel"), this);
	connect(b_cancel, SIGNAL(clicked()), this, SLOT(reject()));

	grid->addWidget(l_tokenimage, 0, 0);
	grid->addWidget(tokenimage, 0, 1);
	grid->addWidget(l_tokenedit, 1, 0);
	grid->addWidget(tokenedit, 1, 1);
	grid->addWidget(b_ok, 2, 0);
	grid->addWidget(b_cancel, 2, 1);

	tokenimage->setImage(tokenImage);

	show();
	b_cancel->setDefault(false);
	b_ok->setDefault(true);
	kdebugf2();
}

void TokenDialog::getValue(QString &tokenValue)
{
	tokenValue = tokenedit->text();
}

void CreateNotifier::notify(QObject* new_object)
{
	emit objectCreated(new_object);
}

PixmapPreview::PixmapPreview() : QLabel(NULL)
{
}

void PixmapPreview::previewUrl(const QUrl& url)
{
	QString path = url.path();
	QPixmap pix( path );
	if (pix.isNull())
		setText(qApp->translate("PixmapPreview", "This is not an image"));
	else
	{
		QWMatrix mx;
		mx.scale(
			double(width())/double(pix.width()),
			double(height())/double(pix.height()));
		pix = pix.xForm(mx);
		setPixmap(pix);
	}
}

ImageDialog::ImageDialog(QWidget* parent)
	: QFileDialog(parent,"image dialog",true)
{
	PixmapPreview* pp = new PixmapPreview();
	setFilter(qApp->translate("ImageDialog", "Images")+" (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.gif *.GIF *.bmp *.BMP)");
	setContentsPreviewEnabled(true);
	setContentsPreview(pp, pp);
	setPreviewMode(QFileDialog::Contents);
}


QValueList<int> toIntList(const QValueList<QVariant> &in)
{
	QValueList<int> out;
	CONST_FOREACH(it, in)
		out.append((*it).toInt());
	return out;
}

QValueList<QVariant> toVariantList(const QValueList<int> &in)
{
	QValueList<QVariant> out;
	CONST_FOREACH(it, in)
		out.append(QVariant(*it));
	return out;
}

QRegExp clean_regexp;
QString toPlainText(const QString &text)
{
	kdebugm(KDEBUG_INFO, "rich: %s\n", text.local8Bit().data());
	if (clean_regexp.isEmpty())
	{
		clean_regexp = QRegExp("<.*>");
		clean_regexp.setMinimal(true);
	}
	QString copy=text;
	copy.replace("\r\n", " ");
	copy.replace("\n",   " ");
	copy.remove(clean_regexp);
	HtmlDocument::unescapeText(copy);
	kdebugm(KDEBUG_INFO, "plain: %s\n", copy.local8Bit().data());
	return copy;
}

QString narg(const QString &s, const QString **tab, int count)
{
	kdebugf();
	QString out;
	const QChar *d = s.unicode();
	const QChar *dend = d + s.length();
	int j = 0;
	char maxc = '0' + count;
	if (count > 9)
		return QString::null;

	while (d != dend)
	{
		if (*d == '%' && d + 1 < dend && *(d + 1) >= '1' && *(d + 1) <= maxc)
		{
			out.append(QConstString(d - j, j).string());
			++d;
			out.append(*(tab[*d - '1']));
			j = 0;
		}
		else
			++j;
		++d;
	}
	out.append(QConstString(d - j, j).string());
//	kdebugm(KDEBUG_DUMP, "out: '%s'\n", out.local8Bit().data());
	kdebugf2();

	return out;
}

QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3, const QString &arg4,
				const QString &arg5, const QString &arg6,
				const QString &arg7, const QString &arg8,
				const QString &arg9)
{
	const QString *tab[9]={&arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9};
	return narg(s, tab, 9);
}

QString narg(const QString &s, const QString &arg1, const QString &arg2, const QString &arg3, const QString &arg4)
{
	const QString *tab[4]={&arg1, &arg2, &arg3, &arg4};
	return narg(s, tab, 4);
}

#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif

void printBacktrace(const QString &header)
{
	if (header.isEmpty())
		fprintf(stderr, "\nbacktrace:\n");
	else
		fprintf(stderr, "\nbacktrace: ('%s')\n", header.local8Bit().data());
#ifdef HAVE_EXECINFO
	void *bt_array[100];
	char **bt_strings;
	int num_entries;
	if ((num_entries = backtrace(bt_array, 100)) < 0) {
		fprintf(stderr, "could not generate backtrace\n");
		return;
	}
	if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL) {
		fprintf(stderr, "could not get symbol names for backtrace\n");
		return;
	}
	fprintf(stderr, "======= BEGIN OF BACKTRACE =====\n");
	for (int i = 0; i < num_entries; ++i)
		fprintf(stderr, "[%d] %s\n", i, bt_strings[i]);
	fprintf(stderr, "======= END OF BACKTRACE  ======\n");
	free(bt_strings);
#else
	fprintf(stderr, "backtrace not available\n");
#endif
	fflush(stderr);
}

LayoutHelper::LayoutHelper() : labels(), riches()
{
}

LayoutHelper::~LayoutHelper()
{
	while (!riches.isEmpty())
	{
		delete riches.last();
		riches.pop_back();
	}
}

void LayoutHelper::addLabel(QLabel *label)
{
	labels.push_back(label);
	riches.push_back(new QSimpleRichText(label->text(), label->font()));
}

void LayoutHelper::resizeLabels()
{
	QValueList<QLabel *>::iterator l = labels.begin(), lend = labels.end();
	QValueList<QSimpleRichText *>::iterator r = riches.begin();

	while (l != lend)
	{
		if ((*l)->isVisible())
		{
			(*r)->setWidth((*l)->width());
			(*l)->setMinimumHeight((*r)->height());
		}
		++l;
		++r;
	}
}

void LayoutHelper::textChanged(QLabel *label)
{
	QValueList<QLabel *>::iterator l = labels.begin(), lend = labels.end();
	QValueList<QSimpleRichText *>::iterator r = riches.begin();

	while (l != lend)
	{
		if ((*l) == label)
		{
			delete *r;
			*r = new QSimpleRichText(label->text(), label->font());
			break;
		}
		++l;
		++r;
	}
}

int showPopupMenu(QPopupMenu *menu)
{
	kdebugf();
	QSize desktopSize = QApplication::desktop()->size();
	QSize menuSizeHint = menu->sizeHint();
	QPoint p = QCursor::pos();
//	kdebugm(KDEBUG_INFO, "p:%d,%d menuSize:%d,%d desktop:%d,%d\n", p.x(), p.y(), menuSizeHint.width(), menuSizeHint.height(), desktopSize.width(), desktopSize.height());
	if (p.y() + menuSizeHint.height() >= desktopSize.height())
		p.setY(p.y() - menuSizeHint.height() - 10);
	else
		p.setY(p.y() + 10);
	if (p.x() + menuSizeHint.width() >= desktopSize.width())
		p.setX(p.x() - menuSizeHint.width() - 10);
	else
		p.setX(p.x() + 10);
//	kdebugm(KDEBUG_INFO, "new_p:%d,%d\n", p.x(), p.y());

	int r = menu->exec(p);
	kdebugf2();
	return r;
}
