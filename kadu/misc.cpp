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
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qprocess.h>
#include <qpushbutton.h>
#include <qregexp.h>
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
#include "misc.h"
#include "userlistelement.h"

#define GG_FONT_IMAGE	0x80

QFont *defaultFont;
QFontInfo *defaultFontInfo;

QTextCodec *codec_cp1250 = QTextCodec::codecForName("CP1250");
QTextCodec *codec_latin2 = QTextCodec::codecForName("ISO8859-2");

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
	}
	return path+subpath;
}

//stat,getcwd
#include <unistd.h>
#include <sys/stat.h>
//getenv
#include <stdlib.h>
//memcpy,strcat,strchr
#include <string.h>
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


	if (argv0[0]=='.' && argv0[1]=='/') //¶cie¿ka wzglêdem bie¿±cego katalogu (./)
	{
		if (getcwd(path, len-2)==NULL)
		{
			path[0]=0;
			kdebugf2();
			return NULL;
		}
		strncat(path, argv0+1, len-1);
		path[len-1]=0;
		lastslash=strrchr(path, '/');
		lastslash[1]=0;
		kdebugf2();
		return path;
	}

	if (argv0[0]=='.' && argv0[1]=='.' && argv0[2]=='/') //¶cie¿ka wzglêdem bie¿±cego katalogu (../)
	{
		if (getcwd(path, len-2)==NULL)
		{
			path[0]=0;
			kdebugf2();
			return NULL;
		}
		strncat(path, "/", len-1);
		strncat(path, argv0, len-1);
		path[len-1]=0;
		lastslash=strrchr(path, '/');
		lastslash[1]=0;
		kdebugf2();
		return path;
	}

	if (argv0[0]=='/') //¶cie¿ka bezwzglêdna
	{
		strncpy(path, argv0, len-1);
		path[len-1]=0;
		lastslash=strrchr(path, '/');
		lastslash[1]=0;
		kdebugf2();
		return path;
	}

	previous=getenv("PATH"); //szukamy we wszystkich katalogach, które s± w PATH
	while((current=strchr(previous, ':')))
	{
		l=current-previous;
		if (l>len-2)
		{
			path[0]=0;
			kdebugf2();
			return NULL;
		}

		memcpy(path, previous, l);
		path[l]='/';
		path[l+1]=0;
		strncat(path, argv0, len);
		path[len-1]=0;
		if (stat(path, &buf)!=-1)
		{
			if (path[l-1]=='/')
				path[l]=0;
			else
				path[l+1]=0;
			kdebugf2();
			return path;
		}
		previous=current+1;
	}
	//nie znale¼li¶my dot±d (bo szukali¶my ':'), wiêc mo¿e w pozosta³ej czê¶ci co¶ siê znajdzie?
	strncpy(path, previous, len-2);
	path[len-2]=0;

	l=strlen(path);
	path[l]='/';
	path[l+1]=0;
	strncat(path, argv0, len);
	path[len-1]=0;
	if (stat(path, &buf)!=-1)
	{
		if (path[l-1]=='/')
			path[l]=0;
		else
			path[l+1]=0;
		kdebugf2();
		return path;
	}
	else
	{
		path[0]=0;
		kdebugf2();
		return NULL;
	}
	kdebugf2();
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

		//je¿eli ¶cie¿ki nie koñcz± siê na /share i /bin oraz gdy bez tych koñcówek
		//¶cie¿ki siê nie pokrywaj±, to znaczy ¿e kto¶ ustawi³ rêcznie DATADIR lub BINDIR
		if (!datadir.endsWith("/share") || !bindir.endsWith("/bin") ||
			(datadir.left(datadir.length() - 6) != bindir.left(bindir.length() - 4)))
		{
			data_path = datadir + "/";
			lib_path = datadir + "../lib/";
		}
		else
		{
			char cpath[1024];
			if (findMe(argv0, cpath, 1024) == NULL)
			{
				data_path = datadir + "/";
				lib_path = datadir + "../lib/";
			}
			else
			{
				data_path = QString(cpath) + "../share/";
				lib_path = QString(cpath) + "../lib/";
			}
		}
#endif
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

QCString unicode2cp(const QString &buf)
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
	tmp.replace("\\x0119", "e");
	tmp.replace("\\x00f3", "o");
	tmp.replace("\\x0105", "a");
	tmp.replace("\\x015b", "s");
	tmp.replace("\\x0142", "l");
	tmp.replace("\\x017c", "z");
	tmp.replace("\\x017a", "z");
	tmp.replace("\\x0107", "c");
	tmp.replace("\\x0144", "n");
	tmp.replace("\\x0118", "E");
	tmp.replace("\\x00d3", "O");
	tmp.replace("\\x0104", "A");
	tmp.replace("\\x015a", "S");
	tmp.replace("\\x0141", "L");
	tmp.replace("\\x017b", "Z");
	tmp.replace("\\x0179", "Z");
	tmp.replace("\\x0106", "C");
	tmp.replace("\\x0143", "N");
	return tmp;
}

QString unicode2latinUrl(const QString &buf)
{
	QString tmp = buf;
	tmp.replace("\\x0119", "%EA");
	tmp.replace("\\x00f3", "%F3");
	tmp.replace("\\x0105", "%B1");
	tmp.replace("\\x015b", "%B6");
	tmp.replace("\\x0142", "%B3");
	tmp.replace("\\x017c", "%BF");
	tmp.replace("\\x017a", "%BC");
	tmp.replace("\\x0107", "%E6");
	tmp.replace("\\x0144", "%F1");
	tmp.replace("\\x0118", "%CA");
	tmp.replace("\\x00d3", "%D3");
	tmp.replace("\\x0104", "%A1");
	tmp.replace("\\x015a", "%A6");
	tmp.replace("\\x0141", "%A3");
	tmp.replace("\\x017b", "%AF");
	tmp.replace("\\x0179", "%AC");
	tmp.replace("\\x0106", "%C3");
	tmp.replace("\\x0143", "%D1");
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
	static const char *local[] = {"en",
		"de",
		"fr",
		"it",
		"pl",  0};

	static const char *name[] ={QT_TR_NOOP("English"),
		QT_TR_NOOP("German"),
		QT_TR_NOOP("French"),
		QT_TR_NOOP("Italian"),
		QT_TR_NOOP("Polish"), 0};

	for (int i = 0; local[i]; ++i)
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
	QProcess *browser;
	QStringList args;

	QString webBrowser=config_file.readEntry("Chat","WebBrowser");
	if (webBrowser.isEmpty())
	{
		QMessageBox::warning(0, qApp->translate("@default", QT_TR_NOOP("WWW error")),
			qApp->translate("@default", QT_TR_NOOP("Web browser was not specified. Visit the configuration section")));
		kdebugmf(KDEBUG_INFO, "Web browser NOT specified.\n");
		return;
	}
	if (!webBrowser.contains("%1"))
		webBrowser.append(" \"%1\"");

	webBrowser.replace("%1", unicode2latinUrl(link));

	args=toStringList("sh", "-c", webBrowser);

	CONST_FOREACH(i, args)
		kdebugmf(KDEBUG_INFO, "%s\n", (*i).local8Bit().data());
	browser = new QProcess(qApp);
	browser->setArguments(args);
	QObject::connect(browser, SIGNAL(processExited()), browser, SLOT(deleteLater()));

	if (!browser->start())
		QMessageBox::critical(0, qApp->translate("@default", QT_TR_NOOP("WWW error")),
			qApp->translate("@default", QT_TR_NOOP("Could not spawn Web browser process. Check if the Web browser is functional")));

	kdebugf2();
}


struct ParseElem
{
	enum {PE_STRING, PE_CHECK_NULL, PE_CHECK_FILE, PE_EXECUTE} type;
	QString str;
};

QString parse(const QString &s, const UserListElement &ule, bool escape)
{
	kdebugmf(KDEBUG_DUMP, "%s escape=%i\n", s.local8Bit().data(), escape);
	int index = 0, i, len = s.length();
	QValueList<ParseElem> parseStack;

	static bool searchChars[256]={false};
	QCString slatin = unicode2latin(s);
	searchChars[(unsigned char)'%']=true;
	searchChars[(unsigned char)'`']=true;
	searchChars[(unsigned char)'[']=true;
	searchChars[(unsigned char)'{']=true;
	searchChars[(unsigned char)'\'']=true;
	searchChars[(unsigned char)'}']=true;
	searchChars[(unsigned char)']']=true;

	while (index<len)
	{
		ParseElem pe1, pe;

		for(i=index; i<len; ++i)
			if (searchChars[(unsigned char)slatin[i]])
				break;
		if (i==len)
			i=-1;

//		this is the same, but code above is muuuuch faster
//		i=s.find(QRegExp("%|`|\\{|\\[|'|\\}|\\]"), index);

		if (i==-1)
		{
			pe1.type=ParseElem::PE_STRING;
			pe1.str=s.mid(index);
			parseStack.push_back(pe1);
			break;
		}
		if (i!=index)
		{
			pe1.type=ParseElem::PE_STRING;
			pe1.str=s.mid(index, i-index);
			parseStack.push_back(pe1);
		}

		QChar c=s[i];
		if (c=='%')
		{
			++i;
			if (i==len)
				break;
			pe.type=ParseElem::PE_STRING;

			switch(slatin[i])
			{
				case 's':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = qApp->translate("@default", ule.status("Gadu").name());
					break;
				case 't':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = ule.status("Gadu").name();
					break;
				case 'd':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = ule.status("Gadu").description();

				 	if (escape)
			 			HtmlDocument::escapeText(pe.str);
					if(config_file.readBoolEntry("Look", "ShowMultilineDesc"))
					{
						pe.str.replace("\n", "<br/>");
						pe.str.replace(QRegExp("\\s\\s"), QString(" &nbsp;"));
					}
					break;
				case 'i':
					++i;
					if (ule.usesProtocol("Gadu") && ule.hasIP("Gadu"))
						pe.str = ule.IP("Gadu").toString();
					break;
				case 'v':
					++i;
					if (ule.usesProtocol("Gadu") && ule.hasIP("Gadu"))
						pe.str = ule.DNSName("Gadu");
					break;
				case 'o':
					++i;
					if (ule.usesProtocol("Gadu") && ule.port("Gadu") == 2)
						pe.str = " ";
					break;
				case 'p':
					++i;
					if (ule.usesProtocol("Gadu") && ule.port("Gadu"))
						pe.str = QString::number(ule.port("Gadu"));
					break;
				case 'u':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = ule.ID("Gadu");
					break;
				case 'n':
					++i;
					pe.str=ule.nickName();
					if(escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'a':
					++i;
					pe.str=ule.altNick();
					if(escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'f':
					++i;
					pe.str=ule.firstName();
					if(escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'r':
					++i;
					pe.str=ule.lastName();
					if(escape)
						HtmlDocument::escapeText(pe.str);
					break;
				case 'm':
					++i;
					pe.str = ule.mobile();
					break;
				case 'g':
					++i;
					pe.str = ule.data("Groups").toStringList().join(",");
					break;
				case 'e':
					++i;
					pe.str = ule.email();
					break;
				case 'x':
					++i;
					if (ule.usesProtocol("Gadu"))
						pe.str = QString::number(ule.protocolData("Gadu", "MaxImageSize").toUInt());
					break;
				case '%':
					++i;
				default:
					pe.str="%";
			}
			parseStack.push_back(pe);
		}
		else if (c=='[')
		{
			++i;
			pe.type=ParseElem::PE_CHECK_NULL;
			parseStack.push_back(pe);
		}
		else if (c==']')
		{
			++i;
			bool anyNull=false;
			while (!parseStack.empty())
			{
				ParseElem &pe2=parseStack.last();
				if (pe2.type==ParseElem::PE_STRING)
				{
					if (pe2.str.isEmpty() || anyNull)
						anyNull=true;
					else
						pe.str.prepend(pe2.str);
					parseStack.pop_back();
				}
				else if (pe2.type==ParseElem::PE_CHECK_NULL)
				{
					parseStack.pop_back();
					if (!anyNull)
					{
						pe.type=ParseElem::PE_STRING;
						parseStack.push_back(pe);
					}
					break;
				}
			}
		}
		else if (c=='{')
		{
			++i;
			pe.type=ParseElem::PE_CHECK_FILE;
			parseStack.push_back(pe);
		}
		else if (c=='}')
		{
			++i;
			while (!parseStack.empty())
			{
				ParseElem &pe2=parseStack.last();
				if (pe2.type==ParseElem::PE_STRING)
				{
					pe.str.prepend(pe2.str);
					parseStack.pop_back();
				}
				else if (pe2.type==ParseElem::PE_CHECK_FILE)
				{
					int f=pe.str.find(' ', 0);
					bool findexist=true;
					parseStack.pop_back();
					QString file;
					if (f==-1)
						file=pe.str;
					else
						file=pe.str.left(f);
					if (!file.isEmpty())
						if (file[0]=='~')
						{
							file=file.mid(1);
							findexist=false;
						}
					pe.str=pe.str.mid(f+1);
					if (QFile::exists(file)==findexist)
					{
						pe.type=ParseElem::PE_STRING;
						parseStack.push_back(pe);
					}
					break;
				}
			}
		}
		else if (c=='`')
		{
			++i;
			pe.type=ParseElem::PE_EXECUTE;
			parseStack.push_back(pe);
		}
		else if (c=='\'')
		{
			++i;
			while (!parseStack.empty())
			{
				ParseElem pe2=parseStack.last();
				if (pe2.type==ParseElem::PE_STRING)
				{
					pe.str.prepend(pe2.str);
					parseStack.pop_back();
				}
				else if (pe2.type==ParseElem::PE_EXECUTE)
				{
					parseStack.pop_back();
					pe.str.remove(QRegExp("`|>|<"));
					pe.str.append(" >");
					pe.str.append(ggPath("execoutput"));

					system(pe.str.local8Bit());
					QFile *f=new QFile(ggPath("execoutput"));
					if (f->open(IO_ReadOnly))
					{
						pe.type=ParseElem::PE_STRING;
						pe.str=QString(f->readAll());
						parseStack.push_back(pe);
						f->close();
						QFile::remove(ggPath("execoutput"));
					}
					delete f;
					break;
				}
			}
		}
		else
			kdebugm(KDEBUG_ERROR, "shit happens? %d %c %d\n", i, (char)c, (char)c);
		index=i;
	}
	QString ret;
	while (!parseStack.empty())
	{
		ParseElem &last=parseStack.last();
		if (last.type==ParseElem::PE_STRING)
			ret.prepend(last.str);
		else
			kdebugm(KDEBUG_WARNING, "Incorrect parse string! %d\n", last.type);
		parseStack.pop_back();
	}
	kdebugm(KDEBUG_DUMP, "%s\n", ret.local8Bit().data());
	return ret;
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
: QDialog(parent, name, false)
{
	kdebugf();
	setCaption(tr("Select description"));

	while (defaultdescriptions.count()>config_file.readUnsignedNumEntry("General", "NumberOfDescriptions"))
		defaultdescriptions.pop_back();

  	desc = new QComboBox(TRUE,this,gadu->status().description());
	desc->insertStringList(defaultdescriptions);

	QLineEdit *ss = new QLineEdit(this, "LineEdit");
	desc->setLineEdit(ss);
	ss->setMaxLength(GG_STATUS_DESCR_MAXSIZE);

	l_yetlen = new QLabel(" "+QString::number(GG_STATUS_DESCR_MAXSIZE - desc->currentText().length()),this);
	connect(desc, SIGNAL(textChanged(const QString&)), this, SLOT(updateYetLen(const QString&)));

	QPixmap pix;
	switch (nr)
	{
		case 1: pix = icons_manager->loadIcon("OnlineWithDescription");	break;
		case 3: pix = icons_manager->loadIcon("BusyWithDescription");	break;
		case 5: pix = icons_manager->loadIcon("InvisibleWithDescription");break;
		case 7: pix = icons_manager->loadIcon("OfflineWithDescription");	break;
		default:pix = icons_manager->loadIcon("OfflineWithDescription");
	}

	QPushButton *okbtn = new QPushButton(QIconSet(pix), tr("&OK"), this);
	QPushButton *cancelbtn = new QPushButton(tr("&Cancel"), this);


	QObject::connect(okbtn, SIGNAL(clicked()), this, SLOT(okbtnPressed()));
	QObject::connect(cancelbtn, SIGNAL(clicked()), this, SLOT(cancelbtnPressed()));

	QGridLayout *grid = new QGridLayout(this, 2, 2);

	grid->addMultiCellWidget(desc, 0, 0, 0, 2);
	grid->addWidget(l_yetlen, 1, 0);
	grid->addWidget(okbtn, 1, 1, Qt::AlignRight);
	grid->addWidget(cancelbtn, 1, 2, Qt::AlignRight);
	grid->addColSpacing(0, 200);

	if (position)
		move(*position);
	resize(250,80);
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
	l_yetlen->setText(" "+QString::number(GG_STATUS_DESCR_MAXSIZE - text.length()));
}

ImageWidget::ImageWidget(QWidget *parent)
	: QWidget(parent, "ImageWidget")
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
	: QDialog(parent, name)
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
	const QChar *d=s.unicode();
	const QChar *dend=d+s.length();
	int j=0;
	char maxc='0'+count;
	if (count>9)
		return QString::null;

	while (d!=dend)
	{
		if (*d=='%' && d+1<dend && *(d+1)>='1' && *(d+1)<=maxc)
		{
			out.append(QString(d-j, j));
			d++;
			out.append(*(tab[*d-'1']));
			j=0;
		}
		else
			j++;
		d++;
	}
	out.append(QString(d-j, j));
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
	fprintf(stderr, "\nbacktrace: '%s'\n", header.local8Bit().data());
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
