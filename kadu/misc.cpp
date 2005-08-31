/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qclipboard.h>
#include <qcolor.h>
#include <qcombobox.h>
#include <qcstring.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qmenudata.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qprocess.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qurl.h>

//getpwuid
#include <pwd.h>

#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "config_file.h"
#include "config_dialog.h"
#include "debug.h"
#include "emoticons.h"
#include "gadu.h"
#include "ignore.h"
#include "kadu.h"
#include "kadu-config.h"
#include "message_box.h"
#include "misc.h"
#include "status.h"
#include "userlist.h"

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
		if (config_dir == NULL)
			path = QString("%1/.gg/").arg(home);
		else
			path = QString("%1/%2/gg/").arg(home).arg(config_dir);
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

QString dataPath(const QString &p, const char *argv0)
{
	static QString path;

	if (argv0!=0)
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
			path = QString(cpath) + "../../";
#else
		QString datadir(DATADIR);
		QString bindir(BINDIR);

		//je¿eli ¶cie¿ki nie koñcz± siê na /share i /bin oraz gdy bez tych koñcówek
		//¶cie¿ki siê nie pokrywaj±, to znaczy ¿e kto¶ ustawi³ rêcznie DATADIR lub BINDIR
		if (!datadir.endsWith("/share") || !bindir.endsWith("/bin") ||
			(datadir.left(datadir.length()-6)!=bindir.left(bindir.length()-4)))
			path=datadir+"/";
		else
		{
			char cpath[1024];
			if (findMe(argv0, cpath, 1024)==NULL)
				path=datadir+"/";
			else
				path=QString(cpath)+"../share/";
		}
#endif
	}
	if (path.isEmpty())
		kdebugm(KDEBUG_PANIC, "dataPath() called _BEFORE_ initial dataPath(\"\",argv[0]) (static object uses dataPath()?) !!!\n");
	kdebugm(KDEBUG_INFO, "%s%s\n", (const char *)path.local8Bit(), (const char *)p.local8Bit());
	return path+p;
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
	tmp.replace(QRegExp("\\x0119"), "e");
	tmp.replace(QRegExp("\\x00f3"), "o");
	tmp.replace(QRegExp("\\x0105"), "a");
	tmp.replace(QRegExp("\\x015b"), "s");
	tmp.replace(QRegExp("\\x0142"), "l");
	tmp.replace(QRegExp("\\x017c"), "z");
	tmp.replace(QRegExp("\\x017a"), "z");
	tmp.replace(QRegExp("\\x0107"), "c");
	tmp.replace(QRegExp("\\x0144"), "n");
	tmp.replace(QRegExp("\\x0118"), "E");
	tmp.replace(QRegExp("\\x00d3"), "O");
	tmp.replace(QRegExp("\\x0104"), "A");
	tmp.replace(QRegExp("\\x015a"), "S");
	tmp.replace(QRegExp("\\x0141"), "L");
	tmp.replace(QRegExp("\\x017b"), "Z");
	tmp.replace(QRegExp("\\x0179"), "Z");
	tmp.replace(QRegExp("\\x0106"), "C");
	tmp.replace(QRegExp("\\x0143"), "N");
	return tmp;
}

QString unicode2latinUrl(const QString &buf)
{
	QString tmp = buf;
	tmp.replace(QRegExp("\\x0119"), "%EA");
	tmp.replace(QRegExp("\\x00f3"), "%F3");
	tmp.replace(QRegExp("\\x0105"), "%B1");
	tmp.replace(QRegExp("\\x015b"), "%B6");
	tmp.replace(QRegExp("\\x0142"), "%B3");
	tmp.replace(QRegExp("\\x017c"), "%BF");
	tmp.replace(QRegExp("\\x017a"), "%BC");
	tmp.replace(QRegExp("\\x0107"), "%E6");
	tmp.replace(QRegExp("\\x0144"), "%F1");
	tmp.replace(QRegExp("\\x0118"), "%CA");
	tmp.replace(QRegExp("\\x00d3"), "%D3");
	tmp.replace(QRegExp("\\x0104"), "%A1");
	tmp.replace(QRegExp("\\x015a"), "%A6");
	tmp.replace(QRegExp("\\x0141"), "%A3");
	tmp.replace(QRegExp("\\x017b"), "%AF");
	tmp.replace(QRegExp("\\x0179"), "%AC");
	tmp.replace(QRegExp("\\x0106"), "%C3");
	tmp.replace(QRegExp("\\x0143"), "%D1");
	return tmp;
}

//wygl±da magicznie, nie? :D
QString unicodeUrl2latinUrl(const QString &buf)
{
	QString tmp = buf;
	tmp.replace(QRegExp("%C4%99"), "%EA"); //ê
	tmp.replace(QRegExp("%C3%B3"), "%F3"); //ó
	tmp.replace(QRegExp("%C4%85"), "%B1"); //±
	tmp.replace(QRegExp("%C5%9B"), "%B6"); //¶
	tmp.replace(QRegExp("%C5%82"), "%B3"); //³
	tmp.replace(QRegExp("%C5%BC"), "%BF"); //¿
	tmp.replace(QRegExp("%C5%BA"), "%BC"); //¼
	tmp.replace(QRegExp("%C4%87"), "%E6"); //æ
	tmp.replace(QRegExp("%C5%84"), "%F1"); //ñ
	tmp.replace(QRegExp("%C4%98"), "%CA"); //Ê
	tmp.replace(QRegExp("%C3%93"), "%D3"); //Ó
	tmp.replace(QRegExp("%C4%84"), "%A1"); //¡
	tmp.replace(QRegExp("%C5%9A"), "%A6"); //¦
	tmp.replace(QRegExp("%C5%81"), "%A3"); //£
	tmp.replace(QRegExp("%C5%BB"), "%AF"); //¯
	tmp.replace(QRegExp("%C5%B9"), "%AC"); //¬
	tmp.replace(QRegExp("%C4%86"), "%C3"); //Æ
	tmp.replace(QRegExp("%C5%83"), "%D1"); //Ñ
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
	const char *local[] = {"en",
		"de",
		"fr",
		"it",
		"pl",  0};

	const char *name[] ={QT_TR_NOOP("English"),
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

	webBrowser.replace(QRegExp("%1"), unicode2latinUrl(link));

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

QString formatGGMessage(const QString &msg, int formats_length, void *formats, UinType sender)
{
	kdebugf();
	QString mesg, tmp;
	bool bold, italic, underline, color, inspan;
	char *cformats = (char *)formats;
	struct gg_msg_richtext_format *actformat;
	struct gg_msg_richtext_color *actcolor;
	struct gg_msg_richtext_image* actimage;

	bold = italic = underline = color = inspan = false;
	unsigned int pos = 0;

	UinsList uins(sender);

	const UserStatus &curStat = gadu->currentStatus();

	/* gdy mamy sendera na li¶cie kontaktów, nie jest on ignorowany,
	   nie jest anononimowy i nasz status na to pozwala, to zezwalamy na obrazki */
	bool receiveImage =
		userlist.containsUin(sender) &&
		!isIgnored(uins) &&
		!userlist.byUinValue(sender).isAnonymous() &&

		(curStat.isOnline() ||	curStat.isBusy() ||
		(curStat.isInvisible() && config_file.readBoolEntry("Chat", "ReceiveImagesDuringInvisibility")));
	kdebugm(KDEBUG_INFO, "formats_length: %d\n", formats_length);
	for (int i = 0; i < formats_length; ++i)
    	    kdebugm(KDEBUG_INFO, ">>%d\n", cformats[i]);
	if (formats_length)
	{
		while (formats_length)
		{
			actformat = (struct gg_msg_richtext_format *)cformats;
			uint16_t tmpposition = gg_fix16(actformat->position);
			kdebugm(KDEBUG_INFO, "position: %d, font: %d\n", tmpposition, actformat->font);
			if (tmpposition > pos)
			{
				tmp = msg.mid(pos, tmpposition - pos);
				HtmlDocument::escapeText(tmp);
				mesg.append(tmp);
				pos = tmpposition;
			}
			else
			{
				if (inspan)
					mesg.append("</span>");
				if (actformat->font & (~GG_FONT_IMAGE))
				{
					inspan = true;
					mesg.append("<span style=\"");
					if (actformat->font & GG_FONT_BOLD)
						mesg.append("font-weight:600;");
					if (actformat->font & GG_FONT_ITALIC)
						mesg.append("font-style:italic;");
					if (actformat->font & GG_FONT_UNDERLINE)
						mesg.append("text-decoration:underline;");
					if (actformat->font & GG_FONT_COLOR)
					{
						mesg.append("color:");
						actcolor = (struct gg_msg_richtext_color *)(cformats
							+ sizeof(struct gg_msg_richtext_format));
						mesg.append(QColor(actcolor->red, actcolor->green, actcolor->blue).name());
					}
					mesg.append("\">");
				}
				else
					inspan = false;
				cformats += sizeof(gg_msg_richtext_format);
				formats_length -= sizeof(gg_msg_richtext_format);
				if (actformat->font & GG_FONT_IMAGE)
				{
					kdebugmf(KDEBUG_INFO, "I got image probably\n");
					actimage = (struct gg_msg_richtext_image*)(cformats);
					uint32_t tmpsize = gg_fix32(actimage->size);
					uint32_t tmpcrc32 = gg_fix32(actimage->crc32);
					kdebugm(KDEBUG_INFO, "Image size: %d, crc32: %d, sender:%d\n", tmpsize, tmpcrc32, sender);

					//ukrywamy siê przed spy'em i ekg2
					if (tmpsize == 20 && (tmpcrc32 == 4567 || tmpcrc32==99))
					{
						kdebugm(KDEBUG_INFO, "%d: scanning for invisibility detected, preparing tactical nuclear missiles ;)\n", sender);
						if (receiveImage)
							gadu->sendImageRequest(sender, tmpsize, tmpcrc32);
					}
					else if (sender!=0)
					{
						kdebugm(KDEBUG_INFO, "Someone sends us an image\n");
						QString file_name =
							gadu_images_manager.getSavedImageFileName(
								tmpsize,
								tmpcrc32);
						if (!file_name.isEmpty())
						{
							kdebugm(KDEBUG_INFO, "This image was already saved\n");
							mesg.append(GaduImagesManager::imageHtml(file_name));
						}
						else
						{
							if (tmpsize<(config_file.readUnsignedNumEntry("Chat", "MaxImageSize")*1024))
							{
								if (receiveImage)
								{
									kdebugm(KDEBUG_INFO, "sending request\n");
									gadu->sendImageRequest(sender, tmpsize, tmpcrc32);
									mesg.append(GaduImagesManager::loadingImageHtml(
											sender,tmpsize,tmpcrc32));
								}
								else
									mesg.append(qApp->translate("@default", QT_TR_NOOP("###IMAGE BLOCKED###")));
							}
							else
								mesg.append(qApp->translate("@default", QT_TR_NOOP("###IMAGE TOO BIG###")));
						}
					}
					else
					{
						kdebugm(KDEBUG_INFO, "This is my message and my image\n");
						QString file_name =
							gadu_images_manager.getImageToSendFileName(
								tmpsize,
								tmpcrc32);
						mesg.append(GaduImagesManager::imageHtml(file_name));
					}
					cformats += sizeof(gg_msg_richtext_image);
					formats_length -= sizeof(gg_msg_richtext_image);
				}
				else
				{
					cformats += sizeof(gg_msg_richtext_color) * ((actformat->font & GG_FONT_COLOR) != 0);
					formats_length -= sizeof(gg_msg_richtext_color) * ((actformat->font & GG_FONT_COLOR) != 0);
				}
			}
		}
		if (pos < msg.length())
		{
			tmp = msg.mid(pos, msg.length() - pos);
			HtmlDocument::escapeText(tmp);
			mesg.append(tmp);
		}
		if (inspan)
			mesg.append("</span>");
	}
	else
	{
		mesg = msg;
		HtmlDocument::escapeText(mesg);
	}
	kdebugf2();
	return mesg;
}

struct attrib_formant
{
	QString name;
	QString value;
};

struct richtext_formant
{
	struct gg_msg_richtext_format format;
	struct gg_msg_richtext_color color;
	struct gg_msg_richtext_image image;
};

QString stripHTMLFromGGMessage(const QString &msg)
{
	kdebugf();
	QRegExp regexp;
	QString mesg = msg;

//	mesg.replace(QRegExp("^<html><head><meta\\sname=\"qrichtext\"\\s*\\s/></head>"), "");
	mesg.replace(QRegExp("^<html><head>.*<body\\s.*\">\\r\\n"), "");
	mesg.replace(QRegExp("\\r\\n</body></html>\\r\\n$"), "");
	mesg.replace(QRegExp("<wsp>"), "");
	mesg.replace(QRegExp("</wsp>"), "");

	mesg.replace(QRegExp("<p>"), "");
	mesg.replace(QRegExp("<p dir=\"ltr\">"), "");
//	mesg.replace(QRegExp("<p dir=\"rtl\">"), "");
	mesg.replace(QRegExp("</p>"), "");
	regexp.setMinimal(true);
	regexp.setPattern("<font (face=\"(\\S)+\"\\s)?(size=\"\\d{1,2}\"(\\s)?)?(style=\"font-size:\\d{1,2}pt\"(\\s)?)?>");
	mesg.replace(regexp, "");
	mesg.replace(QRegExp("</font>"), "");
	HtmlDocument::unescapeText(mesg);

	return mesg;
}

/**
 * Translates QValueList with formants into flat buffer on heap
 *
 * Precondition - formats_length must contain valid length of result buffer
 */
void *allocFormantBuffer(const QValueList<struct richtext_formant> &formants, int &formats_length)
{
	kdebugf();
	struct gg_msg_richtext richtext_header;
	char *cformats, *tmpformats;

	richtext_header.flag = 2;
	richtext_header.length = gg_fix16(formats_length);
	formats_length += sizeof(struct gg_msg_richtext);
	cformats = new char[formats_length];
	tmpformats = cformats;
	memcpy(tmpformats, &richtext_header, sizeof(struct gg_msg_richtext));
	tmpformats += sizeof(struct gg_msg_richtext);
	CONST_FOREACH(it, formants)
	{
		struct richtext_formant actformant = (*it);
		actformant.format.position = gg_fix16(actformant.format.position);
		memcpy(tmpformats, &actformant, sizeof(gg_msg_richtext_format));
		tmpformats += sizeof(gg_msg_richtext_format);
		if (actformant.format.font & GG_FONT_COLOR)
		{
			memcpy(tmpformats, &actformant.color, sizeof(gg_msg_richtext_color));
			tmpformats += sizeof(gg_msg_richtext_color);
		}
		if (actformant.format.font & GG_FONT_IMAGE)
		{
			memcpy(tmpformats, &actformant.image, sizeof(gg_msg_richtext_image));
			tmpformats += sizeof(gg_msg_richtext_image);
		}
	}
	kdebugmf(KDEBUG_INFO, "formats_length=%d, tmpformats-cformats=%d\n",
		formats_length, tmpformats - cformats);

	return (void *)cformats;
}

QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats)
{
	kdebugf();
	QString mesg, tmp;
	QStringList attribs;
	struct attrib_formant actattrib;
	QValueList<attrib_formant> formantattribs;
	int pos, idx, inspan;
	struct richtext_formant actformant, lastformant;
	QValueList<struct richtext_formant> formants;
	bool endspan;

	mesg = stripHTMLFromGGMessage(msg);

	kdebugmf(KDEBUG_INFO, "\n%s\n", mesg.local8Bit().data());

	inspan = -1;
	pos = idx = formats_length = 0;
	endspan = false;
	lastformant.format.font = 0;

	while (uint(pos) < mesg.length())
	{
		// get indexes of unparsed tags
		int image_idx    = mesg.find("[IMAGE ", pos);
		int span_idx     = mesg.find("<span style=", pos);
		int span_end_idx = mesg.find("</span>", pos);

		// if image(s) was parsed recently, we possibly have to restore previous
		// active formatting (since image formant invalidates it)
		// the following code inserts formant saved in lastformant object
		if (lastformant.format.font != 0 &&
			pos != image_idx && pos != span_idx && pos != span_end_idx)
		{
			lastformant.format.position = pos;	// we need to update position
			formants.append(lastformant);
			formats_length += sizeof(struct gg_msg_richtext_format);
		}
		lastformant.format.font = 0; // don't insert this formant again

		// do we have an image preceding any <span> tags?
		if (image_idx != -1 &&
			(span_idx == -1 || image_idx < span_idx) &&
			(span_end_idx == -1 || image_idx < span_end_idx))
		{
			// we have to translate any unhandled </span> tags before image
			// by inserting empty formant 0
			// (fixes mantis bug 355)
			if (endspan && inspan == -1 && pos)
			{
				endspan = false;	// mark </span> as handled
				actformant.format.position = pos;
				actformant.format.font = 0;
				formants.append(actformant);
				formats_length += sizeof(struct gg_msg_richtext_format);
			}

			// parse [IMAGE] tag and remove it from message
			int idx_end = mesg.find("]", image_idx);
			if (idx_end == -1)
				idx_end = mesg.length() - 1;
			QString file_name = mesg.mid(image_idx+7, idx_end-image_idx-7);
			uint32_t size;
			uint32_t crc32;
			gadu_images_manager.addImageToSend(file_name, size, crc32);
			mesg.remove(image_idx, idx_end-image_idx+1);

			// search for last non-image formant before currently parsed image
			// we need to save it, and reinsert after image in next loop iteration
			// (this is required, since image formant removes any active formatting
			// options)
			QValueList<struct richtext_formant>::const_iterator it = formants.end();
			while (it != formants.begin())
			{
				--it;
				// check for non-image formants (formant 0 is ok)
				if (((*it).format.font & GG_FONT_IMAGE) == 0)
				{
					lastformant = *it;
					break;
				}
			}

			// insert the actual image formant into the list
			actformant.format.position = image_idx;
			actformant.format.font = GG_FONT_IMAGE;
			actformant.image.unknown1 = 0x0109;
			actformant.image.size = gg_fix32(size);
			actformant.image.crc32 = gg_fix32(crc32);
			formants.append(actformant);
			formats_length += sizeof(struct gg_msg_richtext_format)
				+ sizeof(struct gg_msg_richtext_image);
			pos = image_idx;
		}
		else if (inspan == -1)
		{
			// parsing <span> tag (NOTE: we actually handle </span> here too)
			idx = span_idx;
			if (idx != -1)
			{
				kdebugmf(KDEBUG_INFO, "idx=%d\n", idx);
				inspan = idx;

				// close any unhandled </span> tags (insert empty formant)
				if (pos && idx > pos)
				{
					endspan = false;	// mark </span> as handled
					actformant.format.position = pos;
					actformant.format.font = 0;
					formants.append(actformant);
					formats_length += sizeof(struct gg_msg_richtext_format);
				}

				// parse <span> attributes and initialize formant structure
				pos = idx;
				idx = mesg.find("\">", pos);
				tmp = mesg.mid(pos, idx - pos);
				idx += 2;
				mesg.remove(pos, idx - pos);
				tmp = tmp.section("\"", 1, 1);
				attribs = QStringList::split(";", tmp);
				formantattribs.clear();
				CONST_FOREACH(attrib, attribs)
				{
					actattrib.name = (*attrib).section(":", 0, 0);
					actattrib.value = (*attrib).section(":", 1, 1);
					formantattribs.append(actattrib);
				}
				actformant.format.position = pos;
				actformant.format.font = 0;
				CONST_FOREACH(actattrib, formantattribs)
				{
					if ((*actattrib).name == "font-style" && (*actattrib).value == "italic")
						actformant.format.font |= GG_FONT_ITALIC;
					if ((*actattrib).name == "text-decoration" && (*actattrib).value == "underline")
						actformant.format.font |= GG_FONT_UNDERLINE;
					if ((*actattrib).name == "font-weight" && (*actattrib).value == "600")
						actformant.format.font |= GG_FONT_BOLD;
					if ((*actattrib).name == "color")
					{
						actformant.format.font |= GG_FONT_COLOR;
						QColor color((*actattrib).value);
						actformant.color.red = color.red();
						actformant.color.green = color.green();
						actformant.color.blue = color.blue();
					}
				}

				// insert <span> formant into list
				formants.append(actformant);
				formats_length += sizeof(struct gg_msg_richtext_format)
					+ sizeof(struct gg_msg_richtext_color)
					* ((actformant.format.font & GG_FONT_COLOR) != 0);
			}
			else
				break;
		}
		else
		{
			// found a </span> tag
			idx = span_end_idx;
			if (idx != -1)
			{
				// we don't create the formant structure here
				// </span> tag is removed from string, empty formant
				// is inserted in next loop iteration in code above.
				kdebugmf(KDEBUG_INFO, "idx=%d\n", idx);
				pos = idx;
				mesg.remove(pos, 7);
				inspan = -1;
				endspan = true;	// we'll take care of this </span> later
			}
			else
				break;
		}
	}

	// if loop ended before we could insert </span> formant, insert it now
	if (pos && idx == -1)
	{
		actformant.format.position = pos;
		actformant.format.font = 0;
		formants.append(actformant);
		formats_length += sizeof(struct gg_msg_richtext_format);
	}

	// now convert QValueList into flat memory buffer
	if (formats_length)
		formats = allocFormantBuffer(formants, formats_length);
	else
		formats = NULL;

	kdebugmf(KDEBUG_INFO|KDEBUG_FUNCTION_END, "\n%s\n", unicode2latin(mesg).data());
	return mesg;
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

	UinType myUin=config_file.readNumEntry("General", "UIN");
	while (index<len)
	{
		ParseElem pe1, pe;

		for(i=index; i<len; ++i)
			if (searchChars[(unsigned char)slatin[i]])
				break;
		if (i==len)
			i=-1;

//		to jest dok³adnie to samo, tyle ¿e to co wy¿ej jest duuuuu¿o szybsze
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
					if (ule.uin())
						pe.str = qApp->translate("@default", ule.status().name());
					break;
				case 't':
					++i;
					if (ule.uin())
						pe.str = ule.status().name();
					break;
				case 'd':
					++i;
					if (myUin == ule.uin())
						pe.str = gadu->status().description();
					else
						pe.str = ule.status().description();

				 	if (escape)
			 			HtmlDocument::escapeText(pe.str);
					if(config_file.readBoolEntry("Look", "ShowMultilineDesc"))
					{
						pe.str.replace(QRegExp("\n"), QString("<br/>"));
						pe.str.replace(QRegExp("\\s\\s"), QString(" &nbsp;"));
					}
					break;
				case 'i': ++i; if (ule.ip().ip4Addr()) pe.str=ule.ip().toString();         break;
				case 'v': ++i; if (ule.ip().ip4Addr()) pe.str=ule.dnsName();               break;
				case 'o': ++i; if (ule.port()==2)      pe.str=" ";                         break;
				case 'p': ++i; if (ule.port())         pe.str=QString::number(ule.port()); break;
				case 'u': ++i; if (ule.uin())          pe.str=QString::number(ule.uin());  break;
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
				case 'm': ++i; pe.str=ule.mobile();	break;
				case 'g': ++i; pe.str=ule.group();	break;
				case 'e': ++i; pe.str=ule.email();	break;
				case 'x': ++i; pe.str=QString::number(ule.maxImageSize());	break;
				case '%': ++i;
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
					pe.str.replace(QRegExp("`|>|<"), "");
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

ChooseDescription::ChooseDescription ( int nr, QWidget * parent, const char * name)
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
		case 1: pix = icons_manager.loadIcon("OnlineWithDescription");	break;
		case 3: pix = icons_manager.loadIcon("BusyWithDescription");	break;
		case 5: pix = icons_manager.loadIcon("InvisibleWithDescription");break;
		case 7: pix = icons_manager.loadIcon("OfflineWithDescription");	break;
		default:pix = icons_manager.loadIcon("OfflineWithDescription");
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

IconsManager::IconsManager(const QString& name, const QString& configname)
	:Themes(name, configname, "icons_manager")
{
	kdebugf();
	kdebugf2();
}

QString IconsManager::iconPath(const QString &name) const
{
	QString fname;
	if(name.contains('/'))
		fname = name;
	else
		fname = themePath() + getThemeEntry(name);
	return fname;
}

const QPixmap &IconsManager::loadIcon(const QString &name)
{
//	kdebugf();
	QMap<QString, QPixmap>::const_iterator i = icons.find(name);
	if (i != icons.end())
	{
//		kdebugf2();
		return *i;
	}

	QPixmap p;
	if (!p.load(iconPath(name)))
		kdebugmf(KDEBUG_WARNING, "warning - pixmap '%s' cannot be loaded!\n", name.local8Bit().data());
	icons[name] = p;
//	kdebugf2();
	return icons[name];
}

void IconsManager::clear()
{
	kdebugf();
	icons.clear();
	kdebugf2();
}

void IconsManager::registerMenu(QMenuData *menu)
{
	kdebugf();
	menus.push_front(qMakePair(menu, QValueList<QPair<QString, QString> >()));
	kdebugf2();
}

void IconsManager::unregisterMenu(QMenuData *menu)
{
	kdebugf();
	FOREACH(it, menus)
		if ((*it).first==menu)
		{
			menus.remove(it);
			break;
		}
	kdebugf2();
}

void IconsManager::registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName)
{
	kdebugf();
	FOREACH(it, menus)
		if ((*it).first==menu)
		{
			(*it).second.push_front(qMakePair(caption, iconName));
			break;
		}
	kdebugf2();
}

void IconsManager::unregisterMenuItem(QMenuData *menu, const QString &caption)
{
	kdebugf();
	FOREACH(it, menus)
		if ((*it).first==menu)
		{
			FOREACH(it2, (*it).second)
				if ((*it2).first==caption)
				{
					(*it).second.remove(*it2);
					break;
				}
			break;
		}
	kdebugf2();
}

void IconsManager::refreshMenus()
{
	kdebugf();
	FOREACH(it, menus)
	{
		QMenuData *menu=(*it).first;
		for (unsigned int i = 0, count = menu->count(); i < count; ++i)
		{
			int id = menu->idAt(i);
			QString t = menu->text(id);

			FOREACH(it2, (*it).second)
				//startsWith jest potrzebne, bo je¿eli opcja w menu ma skrót klawiszowy,
				//to menu->text(id) zwraca napis "Nazwa opcji\tskrót klawiszowy"
				if (t == (*it2).first || t.startsWith((*it2).first + "\t"))
				{
					bool enabled = menu->isItemEnabled(id);
					bool checked = menu->isItemChecked(id);
					menu->changeItem(id, loadIcon((*it2).second), t);
					menu->setItemEnabled(id, enabled);
					menu->setItemChecked(id, checked);
				}
		}
	}
	kdebugf2();
}

void IconsManager::onApplyConfigDialog()
{
	kdebugf();
	QString previousIconTheme=config_file.readEntry("Look", "IconTheme");
	QComboBox *iconThemeCombo= ConfigDialog::getComboBox("Look", "Icon theme");
	QString selectedTheme;
	if (iconThemeCombo->currentText() == tr("Default"))
		selectedTheme="default";
	else
	    selectedTheme=iconThemeCombo->currentText();

	if (selectedTheme!=previousIconTheme)
	{
		config_file.writeEntry("Look", "IconTheme", selectedTheme);

		icons_manager.clear();
		icons_manager.setTheme(selectedTheme);
		ToolBar::refreshIcons();
		UserBox::userboxmenu->refreshIcons();
		icons_manager.refreshMenus();
		kadu->changeAppearance();
		QMessageBox::information(0, tr("Icons"), tr("Please close all (except main) Kadu windows"));
	}

	config_file.writeEntry("Look", "IconsPaths", icons_manager.additionalPaths().join(";"));
	kdebugf2();
}

void IconsManager::onCreateConfigDialog()
{
	kdebugf();

	QComboBox *iconThemeCombo=ConfigDialog::getComboBox("Look", "Icon theme");
	iconThemeCombo->insertStringList(icons_manager.themes());
	iconThemeCombo->setCurrentText(config_file.readEntry("Look", "IconTheme"));

	if (icons_manager.themes().contains("default"))
		iconThemeCombo->changeItem(tr("Default"), icons_manager.themes().findIndex("default"));

	QStringList pl(QStringList::split(";", config_file.readEntry("Look", "IconsPaths")));
	ConfigDialog::getSelectPaths("Look", "Icon paths")->setPathList(pl);

	kdebugf2();
}

void IconsManager::initModule()
{
	kdebugf();
	icons_manager_ptr = new IconsManager ("icons", "icons.conf");
	config_file.addVariable("Look", "IconsPaths", "");
	config_file.addVariable("Look", "IconTheme", "default");

	icons_manager.setPaths(QStringList::split(";", config_file.readEntry("Look", "IconsPaths")));

	QStringList themes=icons_manager.themes();
	if (!themes.contains(config_file.readEntry("Look","IconTheme")) && !themes.isEmpty())
		config_file.writeEntry("Look", "IconTheme", themes[0]);

	icons_manager.setTheme(config_file.readEntry("Look","IconTheme"));

	ConfigDialog::addTab("General", "GeneralTab");
	ConfigDialog::addTab("ShortCuts", "ShortCutsTab");
	ConfigDialog::addTab("Chat", "ChatTab");
	ConfigDialog::addTab("Look", "LookTab");
	ConfigDialog::addHBox("Look", "Look", "icon_theme");
	ConfigDialog::addComboBox("Look", "icon_theme", QT_TRANSLATE_NOOP("@default","Icon theme"));
	ConfigDialog::addSelectPaths("Look", "icon_theme", QT_TRANSLATE_NOOP("@default","Icon paths"));

	ConfigDialog::registerSlotOnCreate(&icons_manager, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(&icons_manager, SLOT(onApplyConfigDialog()));
	ConfigDialog::connectSlot("Look", "Icon paths", SIGNAL(changed(const QStringList&)), &icons_manager, SLOT(selectedPaths(const QStringList&)));
	kdebugf2();
}

void IconsManager::selectedPaths(const QStringList& paths)
{
	kdebugf();
	setPaths(paths);
	QComboBox* iconThemeCombo = ConfigDialog::getComboBox("Look","Icon theme");
	QString current = iconThemeCombo->currentText();

	ConfigDialog::getSelectPaths("Look","Icon paths")->setPathList(additionalPaths());

	iconThemeCombo->clear();
	iconThemeCombo->insertStringList(themes());
	iconThemeCombo->setCurrentText(current);

	if (paths.contains("default"))
		iconThemeCombo->changeItem(tr("Default"), paths.findIndex("default"));
	kdebugf2();
}

IconsManager *icons_manager_ptr;
/********** HttpClient **********/

HttpClient::HttpClient()
{
	kdebugf();
	connect(&Socket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(&Socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(&Socket, SIGNAL(connectionClosed()), this, SLOT(onConnectionClosed()));
	kdebugf2();
}

void HttpClient::onConnected()
{
	kdebugf();
	QString query = PostData.isEmpty() ? "GET": "POST";
	query += " ";

	if (Path.left(7) != "http://" && config_file.readBoolEntry("Network", "UseProxy"))
		query += "http://" + Host;

	if ((Path.isEmpty() || Path[0] != '/') && Path.left(7) != "http://")
		query += '/';


	query += Path;
	query += " HTTP/1.1\r\n";
	query += "Host: " + Host + "\r\n";
	query += "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.4) Gecko/20030617\r\n";
//	query += "Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8\r\n";
//	query += "Connection: keep-alive\r\n";
	if (!Referer.isEmpty())
		query += "Referer: " + Referer + "\r\n";
	if (!Cookies.isEmpty())
	{
		query += "Cookie: ";
		CONST_FOREACH(it, Cookies)
		{
			query += it.key() + "=" + it.data();
			++it;
			if (it != Cookies.end())
				query += "; ";
			--it;
		}
		query += "\r\n";
	}
	if (!PostData.isEmpty())
	{
		query += "Content-Type: application/x-www-form-urlencoded\r\n";
		query += "Content-Length: " + QString::number(PostData.size()) + "\r\n";
	}
	query += "\r\n";
	if (!PostData.isEmpty())
		query += QString(PostData);
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Sending query:\n%s\n", query.local8Bit().data());
	Socket.writeBlock(query.local8Bit().data(), query.length());
	kdebugf2();
}

void HttpClient::onReadyRead()
{
	kdebugf();
	int size = Socket.bytesAvailable();
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Data Block Retreived: %i bytes\n", size);
	// Dodaj nowe dane do starych
	char buf[size];
	Socket.readBlock(buf, size);
	//
//	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "%s\n",buf);
	//
	int old_size = Data.size();
	Data.resize(old_size + size);
	for(int i = 0; i < size; ++i)
		Data[old_size + i] = buf[i];
	// Jesli nie mamy jeszcze naglowka
	if (!HeaderParsed)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Trying to parse header\n");
		// Kontynuuj odczyt jesli naglowek niekompletny
		QString s = QString(Data);
		int p = s.find("\r\n\r\n");
		if (p < 0)
			return;
		// Dostalismy naglowek,
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Http header found:\n%s\n", s.local8Bit().data());
		HeaderParsed = true;
		// Wyci±gamy status
		QRegExp status_regexp("HTTP/1\\.[01] (\\d+)");
		if (status_regexp.search(s) < 0)
		{
			Socket.close();
			emit error();
			return;
		}
		StatusCode = status_regexp.cap(1).toInt();
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: StatusCode: %i\n", StatusCode);
		// StatusCode 302 oznacza przekierowanie.
		if (StatusCode == 302)
		{
			QRegExp location_regexp("Location: ([^\\r\\n]+)");
			if (location_regexp.search(s) < 0)
			{
				Socket.close();
				emit error();
				return;
			}
			QString location = location_regexp.cap(1);
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Jumping to %s\n", location.local8Bit().data());
			// czekamy. zbyt szybkie przekierowanie konczy sie
			// czasem petla. nie wiem dlaczego.
			QTime* t = new QTime();
			t->start();
			while (t->elapsed() < 500) {};
			delete t;
			//
			emit redirected(location);
			get(location);
			return;
		}
		// Wyci±gamy Content-Length
		QRegExp cl_regexp("Content-Length: (\\d+)");
		ContentLengthNotFound = cl_regexp.search(s) < 0;
		if (ContentLengthNotFound)
		{
			ContentLength = 0;
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Content-Length not found. We will wait for connection to close.");
		}
		else
		{
			ContentLength = cl_regexp.cap(1).toUInt();
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Content-Length: %i bytes\n",ContentLength);
		}

		// Wyciagamy ewentualne cookie (dla uproszczenia tylko jedno)
		QRegExp cookie_regexp("Set-Cookie: ([^=]+)=([^;]+);");
		if (cookie_regexp.search(s) >= 0)
		{
			QString cookie_name = cookie_regexp.cap(1);
			QString cookie_val = cookie_regexp.cap(2);
			Cookies.insert(cookie_name, cookie_val);
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Cookie retreived: %s=%s\n", cookie_name.local8Bit().data(), cookie_val.local8Bit().data());
		}
		// Wytnij naglowek z Data
		int header_size = p + 4;
		int new_data_size = Data.size() - header_size;
		for(int i = 0; i < new_data_size; ++i)
			Data[i] = Data[header_size+i];
		Data.resize(new_data_size);
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Header parsed and cutted off from data\n");
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Header size: %i bytes\n", header_size);
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: New data block size: %i bytes\n", new_data_size);
		// Je¶li status jest 100 - Continue to czekamy na dalsze dane
		// (uniewa¿niamy ten nag³owek i czekamy na nastêpny)
		if (StatusCode == 100)
		{
			HeaderParsed = false;
			return;
		}
	}
	// Kontynuuj odczyt jesli dane niekompletne
	// lub je¶li mamy czekaæ na connection close
	if (ContentLength > Data.size() || ContentLengthNotFound)
		return;
	// Mamy cale dane
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: All Data Retreived: %i bytes\n", Data.size());
	Socket.close();
	emit finished();
	kdebugf2();
}

void HttpClient::onConnectionClosed()
{
	kdebugf();
	if (HeaderParsed && ContentLengthNotFound)
		emit finished();
	else
		emit error();
	kdebugf2();
}

void HttpClient::setHost(const QString &host)
{
	Host = host;
	Cookies.clear();
}

void HttpClient::get(const QString &path)
{
	Referer = Path;
	Path = path;
	Data.resize(0);
	PostData.resize(0);
	HeaderParsed = false;

	if(config_file.readBoolEntry("Network", "UseProxy", false))
		Socket.connectToHost(
			config_file.readEntry("Network", "ProxyHost"),
			config_file.readNumEntry("Network", "ProxyPort"));
	else
		Socket.connectToHost(Host, 80);
}

void HttpClient::post(const QString &path, const QByteArray& data)
{
	Referer = Path;
	Path = path;
	Data.resize(0);
	PostData.duplicate(data);
	HeaderParsed = false;

	if(config_file.readBoolEntry("Network", "UseProxy", false))
		Socket.connectToHost(
			config_file.readEntry("Network", "ProxyHost"),
			config_file.readNumEntry("Network", "ProxyPort"));
	else
		Socket.connectToHost(Host, 80);

}

void HttpClient::post(const QString &path,const QString& data)
{
	QByteArray PostData;
	PostData.duplicate(data.local8Bit().data(), data.length());
	post(path, PostData);
}

int HttpClient::status() const
{
	return StatusCode;
}

const QByteArray& HttpClient::data() const
{
	return Data;
}

QString HttpClient::encode(const QString& text)
{
	QString encoded = text;
	QUrl::encode(encoded);
	return encoded;
}

const QString &HttpClient::cookie(const QString& name) const
{
	return Cookies[name];
}

const QMap<QString,QString> &HttpClient::cookies() const
{
	return Cookies;
}

void HttpClient::setCookie(const QString &name, const QString &value)
{
	Cookies[name] = value;
}

void HtmlDocument::escapeText(QString& text)
{
	//UWAGA: &amp; MUSI byæ na pocz±tku!
#if QT_VERSION < 0x030100
	text.replace(QRegExp("&"), "&amp;");
	text.replace(QRegExp("<"), "&lt;");
	text.replace(QRegExp(">"), "&gt;");
	text.replace(QRegExp("\""), "&quot;");
	text.replace(QRegExp("'"), "&apos;");
	text.replace(QRegExp("  "), "&nbsp; ");
#else
	text.replace("&", "&amp;");
	text.replace("<", "&lt;");
	text.replace(">", "&gt;");
	text.replace("\"", "&quot;");
	text.replace("'", "&apos;");
	text.replace("  ", "&nbsp; ");
#endif
}

void HtmlDocument::unescapeText(QString& text)
{
	//UWAGA: &amp; MUSI byæ na koñcu!
#if QT_VERSION < 0x030100
	text.replace(QRegExp("&nbsp;"), " ");
	text.replace(QRegExp("&lt;"), "<");
	text.replace(QRegExp("&gt;"), ">");
	text.replace(QRegExp("&quot;"), "\"");
	text.replace(QRegExp("&apos;"), "'");
	text.replace(QRegExp("&amp;"), "&");
#else
	text.replace("&nbsp;", " ");
	text.replace("&lt;", "<");
	text.replace("&gt;", ">");
	text.replace("&quot;", "\"");
	text.replace("&apos;", "'");
	text.replace("&amp;", "&");
#endif
}

void HtmlDocument::addElement(Element e)
{
	unescapeText(e.text);
	Elements.append(e);
}

void HtmlDocument::addTag(const QString &text)
{
	Element e;
	e.text = text;
	e.tag = true;
	Elements.append(e);
}

void HtmlDocument::addText(const QString &text)
{
	Element e;
	e.text = text;
	e.tag = false;
	Elements.append(e);
}

void HtmlDocument::insertTag(const int pos,const QString &text)
{
	Element e;
	e.text = text;
	e.tag = true;
	Elements.insert(Elements.at(pos),e);
}

void HtmlDocument::insertText(const int pos,const QString &text)
{
	Element e;
	e.text = text;
	e.tag = false;
	Elements.append(e);
	Elements.insert(Elements.at(pos),e);
}

void HtmlDocument::parseHtml(const QString& html)
{
	Element e;
	e.tag = false;
	for(unsigned int i = 0, htmllength = html.length(); i < htmllength; ++i)
	{
		const QChar &ch = html[i];
		switch (ch)
		{
			case '<':
				if (!e.tag)
				{
					if (!e.text.isEmpty())
						addElement(e);
					e.tag = true;
					e.text = ch;
				}
				break;
			case '>':
				if (e.tag)
				{
					e.text += ch;
					addElement(e);
					e.tag = false;
					e.text.truncate(0);
				}
				break;
			default:
				e.text += ch;
		}
	}
	if (!e.text.isEmpty())
		addElement(e);
}

QString HtmlDocument::generateHtml() const
{
	QString html,tmp;
	CONST_FOREACH(e, Elements)
	{
		tmp = (*e).text;
		if (!(*e).tag)
			escapeText(tmp);
		html += tmp;
	}
	return html;
}

int HtmlDocument::countElements() const
{
	return Elements.size();
}

bool HtmlDocument::isTagElement(int index) const
{
	return Elements[index].tag;
}

const QString & HtmlDocument::elementText(int index) const
{
	return Elements[index].text;
}

QString & HtmlDocument::elementText(int index)
{
	return Elements[index].text;
}

void HtmlDocument::setElementValue(int index,const QString& text,bool tag)
{
	Element& e=Elements[index];
	e.text=text;
	e.tag=tag;
}

void HtmlDocument::splitElement(int& index,int start,int length)
{
	Element& e=Elements[index];
	if(start>0)
	{
		Element pre;
		pre.tag=e.tag;
		pre.text=e.text.left(start);
		Elements.insert(Elements.at(index),pre);
		++index;
	}
	if(uint(start+length)<e.text.length())
	{
		Element post;
		post.tag=e.tag;
		post.text=e.text.right(e.text.length()-(start+length));
		if(uint(index+1)<Elements.size())
			Elements.insert(Elements.at(index+1),post);
		else
			Elements.append(post);
	}
	e.text=e.text.mid(start,length);
}

void HtmlDocument::convertUrlsToHtml()
{
//	QRegExp url_regexp("(http://|https://|www\\.|ftp://|ftp\\.|sftp://|smb://|file:/|rsync://|mailto:)[a-zA-Z0-9\\-\\._/~?=&#\\+%:;,!@\\\\]+");
	QRegExp url_regexp(latin2unicode((const unsigned char *)"(http://|https://|www\\.|ftp://|ftp\\.|gg:|sftp://|smb://|file:/|rsync://|mailto:|svn://|svn\\+ssh://)[a-zA-Z0-9êó±¶³¿¼æñÊÓ¡¦£¯¬ÆÑ\\-\\._/~?=&#\\+%\\(\\):;,!@\\\\]*"));
	for(int i = 0; i < countElements(); ++i)
	{
		if(isTagElement(i))
			continue;
		QString text=elementText(i);
		int p=url_regexp.search(text);
		if (p < 0)
			continue;
		int l=url_regexp.matchedLength();
		QString link;
		int lft = config_file.readNumEntry("Chat","LinkFoldTreshold");
		QString link2=text.mid(p,l);
		link2.replace(QRegExp("%20"), "%2520");//obej¶cie buga w operze :|, która nie potrafi otworzyæ linka ze spacj±
		if (l-p > lft && config_file.readBoolEntry("Chat","FoldLink"))
			link="<a href=\""+link2+"\">"+text.mid(p,p+(lft/2))+"..."+text.mid(l-(lft/2),lft/2)+"</a>";
		else
			link="<a href=\""+link2+"\">"+text.mid(p,l)+"</a>";
		splitElement(i,p,l);
		setElementValue(i,link,true);
	}
}

/**
	to jest zast±pienie funkcji formatGGMessage,unformatGGMessage,
	ale jeszcze nie u¿ywane i nie wiem czy skoñczone, w ka¿dym
	razie obs³ugê obrazków dorabiam w tamtej funkcji. Adrian.

HtmlDocument GGMessageToHtmlDocument(const QString &msg, int formats_length, void *formats)
{
	QString tmp;
	bool inspan;
	char *cformats = (char *)formats;
	struct gg_msg_richtext_format *actformat;
	struct gg_msg_richtext_color *actcolor;
	int pos, idx;
	HtmlDocument htmldoc;

	kdebugf();
	inspan = false;
	pos = 0;
	if (formats_length) {
		while (formats_length) {
			actformat = (struct gg_msg_richtext_format *)cformats;
			if (actformat->position > pos) {
				tmp = msg.mid(pos, actformat->position - pos);
				htmldoc.addText(tmp);
				pos = actformat->position;
				}
			else {
				if (inspan)
					htmldoc.addTag("</span>");
				if (actformat->font) {
					inspan = true;
					tmp = "<span style=\"";
					if (actformat->font & GG_FONT_BOLD)
						tmp.append("font-weight:600;");
					if (actformat->font & GG_FONT_ITALIC)
						tmp.append("font-style:italic;");
					if (actformat->font & GG_FONT_UNDERLINE)
						tmp.append("text-decoration:underline;");
					if (actformat->font & GG_FONT_COLOR) {
						tmp.append("color:");
						actcolor = (struct gg_msg_richtext_color *)(cformats
							+ sizeof(struct gg_msg_richtext_format));
						tmp.append(QColor(actcolor->red, actcolor->green, actcolor->blue).name());
						}
					tmp.append("\">");
					htmldoc.addTag(tmp);
					}
				else
					inspan = false;
				cformats += sizeof(gg_msg_richtext_format);
				formats_length -= sizeof(gg_msg_richtext_format);
				if (actformat->font & GG_FONT_IMAGE) {
					idx = int((unsigned char)cformats[0]);
					kdebugm(KDEBUG_INFO, "formatGGMessage(): I got image probably: header_length = %d\n",
						idx);
					cformats += idx + 1;
					formats_length -= idx + 1;
					}
				else {
					cformats += sizeof(gg_msg_richtext_color) * ((actformat->font & GG_FONT_COLOR) != 0);
					formats_length -= sizeof(gg_msg_richtext_color) * ((actformat->font & GG_FONT_COLOR) != 0);
					}
				}
			}
		if (pos < msg.length()) {
			tmp = msg.mid(pos, msg.length() - pos);
			htmldoc.addText(tmp);
			}
		if (inspan)
			htmldoc.addTag("</span>");
		}
	else
		htmldoc.addText(msg);
	kdebugf2();
	return htmldoc;
}

void HtmlDocumentToGGMessage(HtmlDocument &htmldoc, QString &msg, int &formats_length, void *&formats)
{
	QString tmp;
	QStringList attribs;
	struct attrib_formant actattrib;
	QValueList<attrib_formant> formantattribs;
	int pos, inspan, it;
	unsigned int i;
	struct gg_msg_richtext richtext_header;
	struct richtext_formant actformant;
	QValueList<struct richtext_formant> formants;
	char *cformats, *tmpformats;

	kdebugf();

	for (it = 0, pos = 0, formats_length = 0, inspan = -1; it < htmldoc.countElements(); ++it) {
		tmp = htmldoc.elementText(it);
		if (htmldoc.isTagElement(it)) {
			kdebugm(KDEBUG_INFO, "HtmlDocumentToGGMessage(): pos = %d\n", pos);
			if (tmp == "</span>")
				inspan = -1;
			else {
				inspan = pos;
				tmp = tmp.section("\"", 1, 1);
				attribs = QStringList::split(";", tmp);
				formantattribs.clear();
				for (i = 0; i < attribs.count(); ++i) {
					actattrib.name = attribs[i].section(":", 0, 0);
					actattrib.value = attribs[i].section(":", 1, 1);
					formantattribs.append(actattrib);
					}
				actformant.format.position = pos;
				actformant.format.font = 0;
				for (i = 0; i < formantattribs.count(); ++i) {
					actattrib = formantattribs[i];
					if (actattrib.name == "font-style" && actattrib.value == "italic")
						actformant.format.font |= GG_FONT_ITALIC;
					if (actattrib.name == "text-decoration" && actattrib.value == "underline")
						actformant.format.font |= GG_FONT_UNDERLINE;
					if (actattrib.name == "font-weight" && actattrib.value == "600")
						actformant.format.font |= GG_FONT_BOLD;
					if (actattrib.name == "color") {
						actformant.format.font |= GG_FONT_COLOR;
						QColor color(actattrib.value);
						actformant.color.red = color.red();
						actformant.color.green = color.green();
						actformant.color.blue = color.blue();
						}
					}
				formants.append(actformant);
				formats_length += sizeof(struct gg_msg_richtext_format)
					+ sizeof(struct gg_msg_richtext_color)
					* ((actformant.format.font & GG_FONT_COLOR) != 0);
				}
			}
		else {
			if (pos && inspan == -1) {
				actformant.format.position = pos;
				actformant.format.font = 0;
				formants.append(actformant);
				formats_length += sizeof(struct gg_msg_richtext_format);
				}
			msg.append(tmp);
			pos += tmp.length();
			}
		}
	if (formats_length) {
		richtext_header.flag = 2;
		richtext_header.length = formats_length;
		formats_length += sizeof(struct gg_msg_richtext);
		cformats = new char[formats_length];
		tmpformats = cformats;
		memcpy(tmpformats, &richtext_header, sizeof(struct gg_msg_richtext));
		tmpformats += sizeof(struct gg_msg_richtext);
		for (QValueList<struct richtext_formant>::iterator it = formants.begin(); it != formants.end(); ++it)
		{
			actformant = (*it);
			if (actformant.format.font & GG_FONT_COLOR) {
				memcpy(tmpformats, &actformant, sizeof(richtext_formant));
				tmpformats += sizeof(richtext_formant);
				}
			else {
				memcpy(tmpformats, &actformant.format, sizeof(gg_msg_richtext_format));
				tmpformats += sizeof(gg_msg_richtext_format);
				}
			}
		kdebugm(KDEBUG_INFO, "HtmlDocumentToGGMessage(): formats_length = %d, tmpformats-cformats = %d\n",
			formats_length, tmpformats - cformats);
		formats = (void *)cformats;
		}
	else
		formats = NULL;

	kdebugm(KDEBUG_INFO, "HtmlDocumentToGGMessage():\n%s\n", unicode2latin(msg).data());
}
*/
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


Themes::Themes(const QString& themename, const QString& configname, const char *name) : QObject(NULL, name)
{
	Name= themename;
	ConfigName= configname;
	ActualTheme="Custom";
}

QStringList Themes::getSubDirs(const QString& path) const
{
	QDir dir(path);
	dir.setFilter(QDir::Dirs);
	QStringList subdirs, dirs=dir.entryList();
	dirs.remove(".");
	dirs.remove("..");
	CONST_FOREACH(dir, dirs)
	{
		QFile s(path+"/"+(*dir)+"/"+ConfigName);
		if (s.exists())
			subdirs.append(*dir);
	}
	return subdirs;
}

const QStringList &Themes::themes() const
{
	return ThemesList;
}

void Themes::setTheme(const QString& theme)
{
	kdebugf();
	if(ThemesList.contains(theme)|| (theme == "Custom"))
	{
		entries.clear();
		ActualTheme= theme;
		if (theme != "Custom")
		{
			ConfigFile theme_file(themePath()+fixFileName(themePath(),ConfigName));
			entries=theme_file.getGroupSection(Name);
		}
		emit themeChanged(ActualTheme);
	}
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_INFO, "end: theme: %s\n", ActualTheme.local8Bit().data());
}

const QString &Themes::theme() const
{
	return ActualTheme;
}

QString Themes::fixFileName(const QString& path, const QString& fn) const
{
	// sprawd¼ czy oryginalna jest ok
	if(QFile::exists(path+"/"+fn))
		return fn;
	// mo¿e ca³o¶æ lowercase?
	if(QFile::exists(path+"/"+fn.lower()))
		return fn.lower();
	// rozbij na nazwê i rozszerzenie
	QString name=fn.section('.',0,0);
	QString ext=fn.section('.',1);
	// mo¿e rozszerzenie uppercase?
	if(QFile::exists(path+"/"+name+"."+ext.upper()))
		return name+"."+ext.upper();
	// nie umiemy poprawiæ, zwracamy oryginaln±
	return fn;
}

void Themes::setPaths(const QStringList& paths)
{
	kdebugf();
	ThemesList.clear();
	ThemesPaths.clear();
	additional.clear();
	QStringList add, temp = paths + defaultKaduPathsWithThemes();
	QFile s;
	CONST_FOREACH(it, temp)
	{
		s.setName((*it)+"/"+ConfigName);
		if (s.exists())
		{
			if (paths.findIndex(*it)!=-1)
				additional.append(*it);
			ThemesPaths.append(*it);
			ThemesList.append((*it).section("/", -2));
		}
		else
			MessageBox::wrn(tr("<i>%1</i><br/>does not contain any theme configuration file").arg(*it));
	}
	emit pathsChanged(ThemesPaths);
	kdebugf2();
}

QStringList Themes::defaultKaduPathsWithThemes() const
{
	QStringList default1, default2;
	default1 = getSubDirs(dataPath("kadu/themes/" + Name));
	default2 = getSubDirs(ggPath(Name));

	FOREACH(it, default1)
		*it = dataPath("kadu/themes/" + Name + "/" + (*it) + "/");

	FOREACH(it, default2)
		*it = ggPath(Name)+"/" + (*it) + "/";

	return default1 + default2;
}

const QStringList &Themes::paths() const
{
    return ThemesPaths;
}

const QStringList &Themes::additionalPaths() const
{
    return additional;
}

QString Themes::themePath(const QString& theme) const
{
	QString t=theme;
	if (theme.isEmpty())
		t= ActualTheme;
	if (theme == "Custom")
		return "";
	if (ThemesPaths.isEmpty())
		return "Custom";
	return ThemesPaths.grep(t).first();
}

QString Themes::getThemeEntry(const QString& name) const
{
	if (entries.contains(name))
		return entries[name];
	else
		return QString("");
}

void CreateNotifier::notify(QObject* new_object)
{
	emit objectCreated(new_object);
}

void GaduImagesManager::setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col)
{
	static QRegExp animRE("<img bgcolor=\"\" animated=\"1\"");
	QString animText = QString("<img bgcolor=\"%1\" animated=\"1\"").arg(col.name());
	for(int i = 0; i < doc.countElements(); ++i)
	{
		if (!doc.isTagElement(i))
			continue;

		QString text = doc.elementText(i);
		text.replace(animRE, animText);
//		kdebugm(KDEBUG_WARNING, ">>%s\n", text.local8Bit().data());
		doc.setElementValue(i, text, true);
	}
}

QString GaduImagesManager::imageHtml(const QString& file_name)
{
	if (file_name.right(4).lower()==".gif")
		return narg(QString("<img bgcolor=\"\" animated=\"1\" src=\"%1\" title=\"%2\"/>"), file_name, file_name);
	else
		return QString("<img src=\"%1\"/>").arg(file_name);
}

QString GaduImagesManager::loadingImageHtml(UinType uin, uint32_t size, uint32_t crc32)
{
	return narg(QString("<img src=\"%1\" gg_sender=\"%2\" gg_size=\"%3\" gg_crc=\"%4\"/>"),
		icons_manager.iconPath("LoadingImage"), QString::number(uin), QString::number(size), QString::number(crc32));
}

void GaduImagesManager::addImageToSend(const QString& file_name, uint32_t& size, uint32_t& crc32)
{
	kdebugf();
	ImageToSend img;
	QFile f(file_name);
	kdebugm(KDEBUG_INFO, "Opening file \"%s\"\n", file_name.local8Bit().data());
	if (!f.open(IO_ReadOnly))
	{
		kdebugm(KDEBUG_ERROR, "Error opening file\n");
		return;
	}
	img.size = f.size();
	img.file_name = file_name;
	img.data = new char[img.size];
	kdebugm(KDEBUG_INFO, "Reading file\n");
	unsigned int ret = f.readBlock(img.data, img.size);
	if (ret != img.size)
		kdebugm(KDEBUG_ERROR, "ret:%d != %d:img.size\n", ret, img.size);
	f.close();
	img.crc32 = gg_crc32(0, (const unsigned char*)img.data, img.size);
	kdebugm(KDEBUG_INFO, "Inserting into images to send: filename=%s, size=%i, crc32=%i\n\n", img.file_name.local8Bit().data(), img.size, img.crc32);
	size = img.size;
	crc32 = img.crc32;
	ImagesToSend[qMakePair(size, crc32)] = img;
	kdebugf2();
}

void GaduImagesManager::sendImage(UinType uin, uint32_t size, uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching images to send: size=%u, crc32=%u, uin=%d\n", size, crc32, uin);
	if (ImagesToSend.contains(qMakePair(size, crc32)))
	{
		ImageToSend &i = ImagesToSend[qMakePair(size, crc32)];
		if (!i.data)
		{
			QFile f(i.file_name);
			if (!f.open(IO_ReadOnly))
			{
				kdebugm(KDEBUG_ERROR, "Error opening file\n");
				return;
			}
			i.data = new char[i.size];
			kdebugm(KDEBUG_INFO, "Reading file\n");
			unsigned int ret = f.readBlock(i.data, i.size);
			if (ret != i.size)
				kdebugm(KDEBUG_ERROR, "ret:%d != %d:img.size\n", ret, i.size);
			f.close();
		}

		gadu->sendImage(uin, i.file_name, i.size, i.data);
		delete[] i.data;
		i.data = NULL;
		i.lastSent = QDateTime::currentDateTime(); // to pole wykorzysta siê przy zapisywaniu listy obrazków do pliku, stare obrazki bêd± usuwane
	}
	else
		kdebugm(KDEBUG_WARNING, "Image data not found\n");
}

QString GaduImagesManager::saveImage(UinType sender, uint32_t size, uint32_t crc32, const QString& filename, const char* data)
{
	kdebugf();
	QString path = ggPath("images");
	kdebugm(KDEBUG_INFO, "Creating directory: %s\n",path.local8Bit().data());
	QDir().mkdir(path);
	QString file_name = QString("%1-%2-%3-%4").arg(sender).arg(size).arg(crc32).arg(filename);
	kdebugm(KDEBUG_INFO, "Saving image as file: %s\n",file_name.local8Bit().data());
	SavedImage img;
	img.size = size;
	img.crc32 = crc32;
	img.file_name = path + "/" + file_name;
	QFile f(img.file_name);
	f.open(IO_WriteOnly);
	f.writeBlock(data,size);
	f.close();
	SavedImages.append(img);
	kdebugf2();
	return img.file_name;
}

QString GaduImagesManager::getImageToSendFileName(uint32_t size, uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching images to send: size=%u, crc32=%u\n",size,crc32);
	CONST_FOREACH(i, ImagesToSend)
	{
		if ((*i).size==size && (*i).crc32==crc32)
		{
			kdebugm(KDEBUG_INFO, "Image data found\n");
			return (*i).file_name;
		}
	}
	kdebugm(KDEBUG_WARNING, "Image data not found\n");
	return "";
}

QString GaduImagesManager::getSavedImageFileName(uint32_t size, uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching saved images: size=%u, crc32=%u\n",size,crc32);
	CONST_FOREACH(i, SavedImages)
	{
		if ((*i).size==size && (*i).crc32==crc32)
		{
			kdebugm(KDEBUG_INFO, "Image data found\n");
			return (*i).file_name;
		}
	}
	kdebugm(KDEBUG_WARNING, "Image data not found\n");
	return "";
}

QString GaduImagesManager::replaceLoadingImages(const QString& text, UinType sender, uint32_t size, uint32_t crc32)
{
	kdebugf();
	QString loading_string = GaduImagesManager::loadingImageHtml(sender,size,crc32);
	QString image_string;
	QString new_text = text;
	int pos;

	QString file_name = getSavedImageFileName(size,crc32);
	if (file_name.right(4).lower()==".gif")
		image_string = narg(QString("<img bgcolor=\"%1\" animated=\"1\" src=\"%2\" title=\"%3\"/>"),
			config_file.readColorEntry("Look","ChatUsrBgColor").name(), file_name, file_name);
	else
		image_string = QString("<img src=\"%1\"/>").arg(file_name);

	while ((pos = new_text.find(loading_string)) != -1)
	{
		kdebugm(KDEBUG_INFO, "Found coresponding loading image at pos %i, replacing\n",pos);
		new_text = new_text.replace(pos, loading_string.length(), image_string);
	}
	kdebugf2();
	return new_text;
}

GaduImagesManager gadu_images_manager;

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

KaduTextBrowser::KaduTextBrowser(QWidget *parent, const char *name)
	: QTextBrowser(parent, name),QToolTip(viewport()),level(0)
{
	kdebugf();
//	setResizePolicy(QScrollView::AutoOne);
//	setWFlags(Qt::WNoAutoErase|Qt::WStaticContents|Qt::WPaintClever);
	connect(this, SIGNAL(linkClicked(const QString&)), this, SLOT(hyperlinkClicked(const QString&)));
	connect(this, SIGNAL(highlighted(const QString&)), this, SLOT(linkHighlighted(const QString &)));
#if QT_VERSION >= 0x030100
	setWrapPolicy(QTextEdit::AtWordOrDocumentBoundary);
#endif
	setTextFormat(Qt::RichText);

//	connect(verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(repaint()));
	kdebugf2();
}

void KaduTextBrowser::maybeTip(const QPoint &c)
{
	if (!highlightedlink.isNull())
		kdebugmf(KDEBUG_INFO, "link %s (X,Y)=%d,%d\n", highlightedlink.local8Bit().data(), c.x(), c.y());
	tip(QRect(c.x()-20,c.y()-5,40,10), highlightedlink);
}

void KaduTextBrowser::linkHighlighted(const QString & link)
{
	highlightedlink = link;
}

void KaduTextBrowser::setSource(const QString &/*name*/)
{
}

void KaduTextBrowser::setMargin(int width)
{
	setMargins(width, width, width, width);
}

void KaduTextBrowser::copyLinkLocation()
{
	kdebugmf(KDEBUG_FUNCTION_START, "anchor = %s\n", anchor.local8Bit().data());
	QApplication::clipboard()->setText(anchor);
}

QPopupMenu *KaduTextBrowser::createPopupMenu(const QPoint &point)
{
	kdebugf();
	anchor = anchorAt(point);
	anchor.replace(QRegExp("%2520"), "%20");//obej¶cie b³êdu w Qt, patrz HtmlDocument::convertUrlsToHtml()

	QPopupMenu* popupmenu = QTextBrowser::createPopupMenu(point);

	if (!anchor.isEmpty())
		popupmenu->insertItem(tr("Copy link &location"), this, SLOT(copyLinkLocation()), CTRL+Key_L, -1, 0);

	kdebugf2();
	return popupmenu;
}

void KaduTextBrowser::drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
{
	/*
		z nie do koñca wiadomych przyczyn, Qt czasami wpada w pêtle i drawContents
		jeszcze	raz wywo³uje sam± siebie, co powoduje wypisanie:
			QPixmap::operator=: Cannot assign to pixmap during painting
			QPaintDevice: Cannot destroy paint device that is being painted
		oraz zawieszenie Kadu (http://www.kadu.net/forum/viewtopic.php?t=2486)
	*/
//	kdebugmf(KDEBUG_INFO, "level: %d\n", level);
	++level;
	if (level==1)
	{
//		kdebugm(KDEBUG_INFO, "x:%d y:%d w:%d h:%d\n", clipx, clipy, clipw, cliph);
		QTextBrowser::drawContents(p, clipx, clipy, clipw, cliph);
//		QTimer::singleShot(0, this, SLOT(repaint()));//niestety konieczne
	}
	--level;
}

void KaduTextBrowser::hyperlinkClicked(const QString &link) const
{
	if (link.startsWith("www."))
		openWebBrowser("http://"+link);
	else
		openWebBrowser(link);
}

void KaduTextBrowser::copy()
{
	kdebugf();

	int paraFrom, indexFrom, paraTo, indexTo;
	getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
//	kdebugm(KDEBUG_DUMP, "selection: %d %d %d %d\n", paraFrom, indexFrom, paraTo, indexTo);
	if (paraFrom==paraTo && indexFrom==indexTo)
		return;

	QString txt=selectedText();
//	kdebugm(KDEBUG_DUMP, "%d    plain:%d rich:%d auto:%d log:%d\n", textFormat(), Qt::PlainText, Qt::RichText, Qt::AutoText, Qt::LogText);
//	kdebugm(KDEBUG_DUMP, "\n%s\n----------------------\n", txt.local8Bit().data());

	//czasem siê to cholerstwo pojawia gdy dostajemy ca³y dokument (bug w qt!),
	//wiêc wypada³oby pozbyæ siê wszystkich zbêdnych tagów...
	txt.replace(QRegExp("<html>"), "");
	txt.replace(QRegExp("</html>"), "");

	txt.replace(QRegExp("<head>"), "");
	txt.replace(QRegExp("</head>"), "");

	txt.replace(QRegExp("<meta[^>]+>"), "");

	txt.replace(QRegExp("<body>"), "");
	txt.replace(QRegExp("<body [^>]+>"), "");
	txt.replace(QRegExp("</body>"), "");

	txt.replace(QRegExp("<a [^>]+>"), "");
	txt.replace(QRegExp("</a>"), "");

	txt.replace(QRegExp("<br>"), "\n");
	txt.replace(QRegExp("<br/>"), "\n");
	txt.replace(QRegExp("<br />"), "\n");

	//usuwamy wszystkie znane tagi htmla, które mog± siê pojawiæ w chacie
	//nie mo¿na u¿yæ po prostu <[^>]+>, bo za³api± siê te¿ emotikony typu <rotfl>
	txt.replace(QRegExp("<![^>]+>"), "");//<!--StartFragment-->

	txt.replace(QRegExp("<p>"), "");
	txt.replace(QRegExp("<p [^>]+>"), "");
	txt.replace(QRegExp("</p>"), "");

	txt.replace(QRegExp("<span>"), "");
	txt.replace(QRegExp("<span [^>]+>"), "");
	txt.replace(QRegExp("</span>"), "");

	txt.replace(QRegExp("<table>"), "");
	txt.replace(QRegExp("<table [^>]+>"), "");
	txt.replace(QRegExp("</table>"), "");

	txt.replace(QRegExp("<tr>"), "");
	txt.replace(QRegExp("<tr [^>]+>"), "");
	txt.replace(QRegExp("</tr>"), "");

	txt.replace(QRegExp("<td>"), "");
	txt.replace(QRegExp("<td [^>]+>"), "");
	txt.replace(QRegExp("</td>"), "");

	//specjalnie traktujemy obrazki, mo¿e u¿ytkownik domy¶li siê o co tu chodzi :P
	txt.replace(QRegExp("<img gg_crc=([0-9]*) gg_sender=([0-9]*) gg_size=([0-9]*) src=[^>]+>"), "\\2-\\3-\\1-*");
	txt.replace(QRegExp("<img src=([^>]+)>"), "\\1");

//	txt.replace(QRegExp("<[^>]+>[^<]+</[^>]+>"), "");

	txt.replace(QRegExp("&lt;"), "<");
	txt.replace(QRegExp("&gt;"), ">");
	txt.replace(QRegExp("&amp;"), "&");
	txt.replace(QRegExp("&quot;"), "\"");

//	kdebugm(KDEBUG_DUMP, "result: \n%s\n\n", txt.local8Bit().data());

	QClipboard *clipboard=QApplication::clipboard();
#if QT_VERSION < 0x030100
	clipboard->setText(txt);
	clipboard->setSelectionMode(!clipboard->selectionModeEnabled());
	clipboard->setText(txt);
#else
	clipboard->setText(txt, QClipboard::Clipboard);
	clipboard->setText(txt, QClipboard::Selection);
#endif
	kdebugf2();
}

void KaduTextBrowser::contentsMouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();
	emit mouseReleased(e, this);
	QTextBrowser::contentsMouseReleaseEvent(e);
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

QRegExp clean_regexps[3];
QString toPlainText(const QString &text)
{
	kdebugm(KDEBUG_INFO, "rich: %s\n", text.local8Bit().data());
	if (clean_regexps[0].isEmpty())
	{
		clean_regexps[0]=QRegExp("\r\n");
		clean_regexps[1]=QRegExp("\n");
		clean_regexps[2]=QRegExp("<.*>");
		clean_regexps[2].setMinimal(true);
	}
	QString copy=text;
	copy.replace(clean_regexps[0], " ");
	copy.replace(clean_regexps[1], " ");
	copy.replace(clean_regexps[2], "");
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
//#if QT_VERSION < 0x030200
//	kdebugm(KDEBUG_DUMP, "'%s'   '%s'  '%s'  '%s'  '%s'\n", s.local8Bit().data(), arg1.local8Bit().data(), arg2.local8Bit().data(), arg3.local8Bit().data(), arg4.local8Bit().data());
	const QString *tab[4]={&arg1, &arg2, &arg3, &arg4};
	return narg(s, tab, 4);
//#else
//	return s.arg(arg1,arg2,arg3,arg4);
//#endif
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
}
