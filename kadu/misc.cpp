#include <qapplication.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qcstring.h>
#include <qdatetime.h>
#include <qregexp.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qaccel.h>
#include <qprocess.h>
#include <qurl.h>
#include <qclipboard.h>
#include <qpopupmenu.h>

#include <pwd.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "misc.h"
#include "config_file.h"
#include "config_dialog.h"
#include "status.h"
#include "gadu.h"
#include "debug.h"
#include "userlist.h"
#include "kadu-config.h"

#define GG_FONT_IMAGE	0x80

QTextCodec *codec_cp1250 = QTextCodec::codecForName("CP1250");
QTextCodec *codec_latin2 = QTextCodec::codecForName("ISO8859-2");

void saveGeometry(QWidget *w, QString section, QString name)
{
	QRect geom;
	geom.setX(w->pos().x());
	geom.setY(w->pos().y());
	geom.setWidth(w->size().width());
	geom.setHeight(w->size().height());
	
	config_file.writeEntry(section, name, geom);
}

void loadGeometry(QWidget *w, QString section, QString name, int defaultX, int defaultY, int defaultWidth, int defaultHeight)
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
char *findMe(const char *argv0, char *path, int len)
{
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
			return NULL;
		}
		strncat(path, argv0+1, len-1);
		path[len-1]=0;
		lastslash=strrchr(path, '/');
		lastslash[1]=0;
		return path;
	}

	if (argv0[0]=='.' && argv0[1]=='.' && argv0[2]=='/') //¶cie¿ka wzglêdem bie¿±cego katalogu (../)
	{
		if (getcwd(path, len-2)==NULL)
		{
			path[0]=0;
			return NULL;
		}
		strncat(path, "/", len-1);
		strncat(path, argv0, len-1);
		path[len-1]=0;
		lastslash=strrchr(path, '/');
		lastslash[1]=0;
		return path;
	}
	
	if (argv0[0]=='/') //¶cie¿ka bezwzglêdna
	{
		strncpy(path, argv0, len-1);
		path[len-1]=0;
		lastslash=strrchr(path, '/');
		lastslash[1]=0;
		return path;
	}

	previous=getenv("PATH"); //szukamy we wszystkich katalogach, które s± w PATH
	while((current=strchr(previous, ':')))
	{
		l=current-previous;
		if (l>len-2)
		{
			path[0]=0;
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
		return path;
	}
	else
	{
		path[0]=0;
		return NULL;
	}
}

QString dataPath(const QString &p, const char *argv0)
{
	static QString path;

	if (argv0!=0)
	{
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
	}
	if (path=="")
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

QString printDateTime(const QDateTime &datetime) {
	QString tmp;
	time_t t;
	QDateTime dt2;
	int delta;

	t = time(NULL);
	dt2.setTime_t(t);
	dt2.setTime(QTime(0, 0));
	tmp = datetime.toString("hh:mm:ss");
	delta = dt2.secsTo(datetime);
//	kdebugm(KDEBUG_INFO, "printDateTime(): %d\n", delta);
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

	if (customtime) {
		date.setTime_t(customtime);
		buf.append(QString(" / S ") + printDateTime(date));
		}

	return buf;
}

QDateTime currentDateTime(void) {
	time_t t;
	QDateTime date;

	t = time(NULL);
	date.setTime_t(t);
	return date;
}

QString pwHash(const QString &tekst) {
	QString nowytekst;
	unsigned int ile;
	unsigned short znak;
	nowytekst = tekst;
	for (ile = 0; ile < tekst.length(); ile++) {
		znak = nowytekst[ile].unicode() ^ ile ^ 1;
		nowytekst[ile] = QChar(znak);
		}
	return nowytekst;
}

QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n)
{
	const char *local[] = {"en",
		"de",
		"it",
		"pl",  0};

	const char *name[] ={QT_TR_NOOP("English"),
		QT_TR_NOOP("German"),
		QT_TR_NOOP("Italian"),
		QT_TR_NOOP("Polish"), 0};

	for (int i = 0; local[i]; i++) {
		if (l2n) {
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

void openWebBrowser(const QString &link) {
	QProcess *browser;
	QString cmd;
	QStringList args;

	if (config_file.readBoolEntry("Chat","DefaultWebBrowser"))
		cmd = QString("konqueror %1").arg(link);
	else {
		if (config_file.readEntry("Chat","WebBrowser") == "") {
			QMessageBox::warning(0, qApp->translate("@default", QT_TR_NOOP("WWW error")),
				qApp->translate("@default", QT_TR_NOOP("Web browser was not specified. Visit the configuration section")));
			kdebugm(KDEBUG_INFO, "openWebBrowser(): Web browser NOT specified.\n");
			return;
			}
		cmd = QString(config_file.readEntry("Chat","WebBrowser")).arg(link);
		}
	args = QStringList::split(" ", cmd);
	for (QStringList::iterator i = args.begin(); i != args.end(); i++)
		kdebugm(KDEBUG_INFO, "openWebBrowser(): %s\n", unicode2latin(*i).data());
	browser = new QProcess();
	browser->setArguments(args);
	if (!browser->start())
		QMessageBox::critical(0, qApp->translate("@default", QT_TR_NOOP("WWW error")),
			qApp->translate("@default", QT_TR_NOOP("Could not spawn Web browser process. Check if the Web browser is functional")));
	delete browser;
}

void escapeSpecialCharacters(QString &msg) {
	msg.replace(QRegExp("&"), "&amp;");
	msg.replace(QRegExp("<"), "&lt;");
	msg.replace(QRegExp(">"), "&gt;");
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
	if (formats_length)
	{
		while (formats_length)
		{
			actformat = (struct gg_msg_richtext_format *)cformats;
			if (actformat->position > pos)
			{
				tmp = msg.mid(pos, actformat->position - pos);
				escapeSpecialCharacters(tmp);
				mesg.append(tmp);
				pos = actformat->position;
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
					kdebugm(KDEBUG_INFO, "formatGGMessage(): I got image probably\n");
					actimage = (struct gg_msg_richtext_image*)(cformats);
					kdebugm(KDEBUG_INFO, QString("Image size: %1, crc32: %2\n").arg(actimage->size).arg(actimage->crc32).local8Bit().data());
					if (sender!=0)
					{
						kdebugm(KDEBUG_INFO, "Someone sends us an image\n");
						QString file_name =
							gadu_images_manager.getSavedImageFileName(
								actimage->size,
								actimage->crc32);
						if(file_name != "")
						{
							kdebugm(KDEBUG_INFO, "This image was already saved\n");
							mesg.append(GaduImagesManager::imageHtml(file_name));
						}
						else
						{
							gadu->sendImageRequest(sender,
								actimage->size,
								actimage->crc32);
							mesg.append(GaduImagesManager::loadingImageHtml(
								sender,actimage->size,actimage->crc32));
						}
					}
					else
					{
						kdebugm(KDEBUG_INFO, "This is my message and my image\n");
						QString file_name =
							gadu_images_manager.getImageToSendFileName(
								actimage->size,
								actimage->crc32);
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
			escapeSpecialCharacters(tmp);
			mesg.append(tmp);
		}
		if (inspan)
			mesg.append("</span>");
	}
	else
	{
		mesg = msg;
		escapeSpecialCharacters(mesg);
	}
	kdebugf2();
	return mesg;
}

struct attrib_formant {
	QString name;
	QString value;
};

struct richtext_formant {
	struct gg_msg_richtext_format format;
	struct gg_msg_richtext_color color;
	struct gg_msg_richtext_image image;
};

QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats) {
	QString mesg, tmp;
	QStringList attribs;
	QRegExp regexp;
	struct attrib_formant actattrib;
	QValueList<attrib_formant> formantattribs;
	int pos, idx, inspan;
	unsigned int i;
	struct gg_msg_richtext richtext_header;
	struct richtext_formant actformant;
	QValueList<struct richtext_formant> formants;
	char *cformats, *tmpformats;

//	kdebugm(KDEBUG_INFO, "unformatGGMessage():\n%s\n", msg.latin1());
	mesg = msg;
//	mesg.replace(QRegExp("^<html><head><meta\\sname=\"qrichtext\"\\s*\\s/></head>"), "");
	mesg.replace(QRegExp("^<html><head>.*<body\\s.*\">\\r\\n"), "");
	mesg.replace(QRegExp("\\r\\n</body></html>\\r\\n$"), "");
	mesg.replace(QRegExp("<p>"), "");
	mesg.replace(QRegExp("<p dir=\"ltr\">"), "");
//	mesg.replace(QRegExp("<p dir=\"rtl\">"), "");
	mesg.replace(QRegExp("</p>"), "");
	regexp.setMinimal(true);
	regexp.setPattern("<font (face=\"(\\S)+\"\\s)?(size=\"\\d{1,2}\"(\\s)?)?(style=\"font-size:\\d{1,2}pt\"(\\s)?)?>");
	mesg.replace(regexp, "");
	mesg.replace(QRegExp("</font>"), "");
	mesg.replace(QRegExp("&quot;"), "\"");
	mesg.replace(QRegExp("&amp;"), "&");
	mesg.replace(QRegExp("&lt;"), "\a");
	mesg.replace(QRegExp("&gt;"), "\f");
//	mesg.replace(QRegExp("&lt;"), "#");
//	mesg.replace(QRegExp("&gt;"), "#");

	kdebugm(KDEBUG_INFO, "unformatGGMessage():\n%s\n", (const char *)unicode2latin(mesg));

	inspan = -1;
	pos = idx = formats_length = 0;
	
	while (uint(pos) < mesg.length()) {
		int image_idx    = mesg.find("[IMAGE ", pos);
		int span_idx     = mesg.find("<span style=", pos);
		int span_end_idx = mesg.find("</span>", pos);
		if (
			image_idx != -1 &&
			(span_idx == -1 || image_idx < span_idx) &&
			(span_end_idx == -1 || image_idx < span_end_idx))
		{
			int idx_end = mesg.find("]",idx);
			if (idx_end == -1)
				idx_end = mesg.length() - 1;
			QString file_name = mesg.mid(image_idx+7,idx_end-image_idx-7);
			uint32_t size;
			uint32_t crc32;
			gadu_images_manager.addImageToSend(file_name,size,crc32);
			mesg.remove(image_idx,idx_end-image_idx+1);
			actformant.format.position = image_idx;
			actformant.format.font = GG_FONT_IMAGE;
			actformant.image.unknown1 = 0x0109;
			actformant.image.size = size;
			actformant.image.crc32 = crc32;
			formants.append(actformant);
			formats_length += sizeof(struct gg_msg_richtext_format)
				+ sizeof(struct gg_msg_richtext_image);
			pos = image_idx;
		}
		else if (inspan == -1) {
			idx = span_idx;
			if (idx != -1) {
				kdebugm(KDEBUG_INFO, "unformatGGMessage(): idx=%d\n", idx);
				inspan = idx;
				if (pos && idx > pos) {
					actformant.format.position = pos;
					actformant.format.font = 0;
					formants.append(actformant);
					formats_length += sizeof(struct gg_msg_richtext_format);
					}
				pos = idx;
				idx = mesg.find("\">", pos);
				tmp = mesg.mid(pos, idx - pos);
				idx += 2;
				mesg.remove(pos, idx - pos);
				tmp = tmp.section("\"", 1, 1);
				attribs = QStringList::split(";", tmp);
				formantattribs.clear();
				for (i = 0; i < attribs.count(); i++) {
					actattrib.name = attribs[i].section(":", 0, 0);
					actattrib.value = attribs[i].section(":", 1, 1);
					formantattribs.append(actattrib);
					}
				actformant.format.position = pos;
				actformant.format.font = 0;
				for (i = 0; i < formantattribs.count(); i++) {
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
			else
				break;
			}
		else {
			idx = span_end_idx;
			if (idx != -1) {
				kdebugm(KDEBUG_INFO, "unformatGGMessage(): idx=%d\n", idx);
				pos = idx;
				mesg.remove(pos, 7);
				inspan = -1;
				}
			else
				break;
			}
		}
	if (pos && idx == -1) {
		actformant.format.position = pos;
		actformant.format.font = 0;
		formants.append(actformant);
		formats_length += sizeof(struct gg_msg_richtext_format);
		}
	if (formats_length) {
		richtext_header.flag = 2;
		richtext_header.length = formats_length;
		formats_length += sizeof(struct gg_msg_richtext);
		cformats = new char[formats_length];
		tmpformats = cformats;
		memcpy(tmpformats, &richtext_header, sizeof(struct gg_msg_richtext));
		tmpformats += sizeof(struct gg_msg_richtext);
		for (QValueList<struct richtext_formant>::iterator it = formants.begin(); it != formants.end(); it++) {
			actformant = (*it);
			memcpy(tmpformats, &actformant, sizeof(gg_msg_richtext_format));
			tmpformats += sizeof(gg_msg_richtext_format);
			if (actformant.format.font & GG_FONT_COLOR) {
				memcpy(tmpformats, &actformant.color, sizeof(gg_msg_richtext_color));
				tmpformats += sizeof(gg_msg_richtext_color);
				}
			if (actformant.format.font & GG_FONT_IMAGE) {
				memcpy(tmpformats, &actformant.image, sizeof(gg_msg_richtext_image));
				tmpformats += sizeof(gg_msg_richtext_image);
				}
			}
		kdebugm(KDEBUG_INFO, "unformatGGMessage(): formats_length=%d, tmpformats-cformats=%d\n",
			formats_length, tmpformats - cformats);
		formats = (void *)cformats;
		}
	else
		formats = NULL;

	mesg.replace(QRegExp("\\a"), "<");
	mesg.replace(QRegExp("\\f"), ">");
//	mesg.replace(QRegExp("#"), "<");
//	mesg.replace(QRegExp("#"), ">");

	kdebugm(KDEBUG_INFO, "unformatGGMessage():\n%s\n", unicode2latin(mesg).data());
	return mesg;
}

struct ParseElem
{
	enum {PE_STRING, PE_CHECK_NULL, PE_CHECK_FILE, PE_EXECUTE} type;
	QString str;
};

QString parse(const QString &s, const UserListElement &ule, bool escape)
{
	kdebugm(KDEBUG_INFO, "parse(): %s escape=%i\n",(const char *)s.local8Bit(), escape);
	int index=0, i, j, len=s.length();
	QValueList<ParseElem> parseStack;

	static bool searchChars[256]={false};
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
		
		for(i=index; i<len; i++)
			if (searchChars[(unsigned char)unicode2latin(s)[i]])
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
			i++;
			if (i==len)
				break;
			pe.type=ParseElem::PE_STRING;
			
			switch(unicode2latin(s)[i]) {
				case 's':
					i++;
					if (!ule.uin)
						break;
					j=statusGGToStatusNr(ule.status);
					if (j%2)
						j--;
					pe.str= qApp->translate("@default", statustext[j]);
					break;
				case 'd':
					i++;
					if(myUin == ule.uin &&
							! ifStatusWithDescription(getCurrentStatus()))
						pe.str=QString::null;
					else
						pe.str=ule.description;

					//pe.str=ule.description;
				 	if (escape)
			 			escapeSpecialCharacters(pe.str);
					if(config_file.readBoolEntry("Look", "ShowMultilineDesc")) {
						pe.str.replace(QRegExp("\n"), QString("<br/>"));
						pe.str.replace(QRegExp("\\s\\s"), QString(" &nbsp;"));
					}
					break;
				case 'i': i++; if (ule.ip.ip4Addr()) pe.str=ule.ip.toString();         break;
				case 'v': i++; if (ule.ip.ip4Addr()) pe.str=ule.dnsname;               break;
				case 'o': i++; if (ule.port==2)      pe.str=" ";                       break;
				case 'p': i++; if (ule.port)         pe.str=QString::number(ule.port); break;
				case 'u': i++; if (ule.uin)          pe.str=QString::number(ule.uin);  break;
				case 'n': i++; pe.str=ule.nickname;		break;
				case 'a': i++; pe.str=ule.altnick;		break;
				case 'f': i++; pe.str=ule.first_name;	break;
				case 'r': i++; pe.str=ule.last_name;	break;
				case 'm': i++; pe.str=ule.mobile;		break;
				case 'g': i++; pe.str=ule.group();		break;
				case 'e': i++; pe.str=ule.email;		break;
				case '%': i++;
				default:
					pe.str="%";
			}
			parseStack.push_back(pe);
		}
		else if (c=='[')
		{
			i++;
			pe.type=ParseElem::PE_CHECK_NULL;
			parseStack.push_back(pe);
		}
		else if (c==']')
		{
			i++;
			bool anyNull=false;
			while (!parseStack.empty())
			{
				ParseElem &pe2=parseStack.last();
				if (pe2.type==ParseElem::PE_STRING)
				{
					if (pe2.str=="" || anyNull)
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
			i++;
			pe.type=ParseElem::PE_CHECK_FILE;
			parseStack.push_back(pe);
		}
		else if (c=='}')
		{
			i++;
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
					if (file.length()>0)
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
			i++;
			pe.type=ParseElem::PE_EXECUTE;
			parseStack.push_back(pe);
		}
		else if (c=='\'')
		{
			i++;
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
	kdebugm(KDEBUG_INFO, "%s\n", (const char *)ret.local8Bit());
	return ret;
}

//internal usage
void stringHeapSortPushDown( QString* heap, int first, int last )
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
void stringHeapSortHelper( QStringList::iterator b, QStringList::iterator e, QString, uint n )
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

	for( uint i = n; i > 0; i-- ) {
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

bool UinsList::equals(const UinsList &uins) const
{
	if (count() != uins.count())
		return false;
	for (UinsList::const_iterator i = begin(); i != end(); i++)
		if(!uins.contains(*i))
			return false;
	return true;
}

UinsList::UinsList() {
}

void UinsList::sort() {
	qHeapSort(*this);
}

ChooseDescription::ChooseDescription ( int nr, QWidget * parent, const char * name)
: QDialog(parent, name, true) {
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Select description"));

  	desc = new QComboBox(TRUE,this,own_description);

	desc->insertStringList(defaultdescriptions);
	QLineEdit *ss;
	ss= new QLineEdit(this,"LineEdit");
	desc->setLineEdit(ss);
	ss->setMaxLength(GG_STATUS_DESCR_MAXSIZE);
	
	l_yetlen = new QLabel(" "+QString::number(GG_STATUS_DESCR_MAXSIZE - desc->currentText().length()),this);
	connect(desc, SIGNAL(textChanged(const QString&)), this, SLOT(updateYetLen(const QString&)));

	QPixmap pix;
	switch (nr) {
		case 1:
			pix = icons_manager.loadIcon("OnlineWithDescription");
			break;
		case 3:
			pix = icons_manager.loadIcon("BusyWithDescription");
			break;
		case 5:
			pix = icons_manager.loadIcon("InvisibleWithDescription");
			break;
		case 7:
			pix = icons_manager.loadIcon("OfflineWithDescription");
			break;
		default:
			pix = icons_manager.loadIcon("OfflineWithDescription");
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
}

void ChooseDescription::okbtnPressed() {
	if (defaultdescriptions.contains(desc->currentText())==0)
	{
		if (defaultdescriptions.count()==4)
			defaultdescriptions.remove(defaultdescriptions.last());
	}
	else 
		defaultdescriptions.remove(desc->currentText());
	defaultdescriptions.prepend(desc->currentText());
	own_description=defaultdescriptions.first();
	accept();
}

void ChooseDescription::cancelbtnPressed() {
	reject();
//	close();
}

void ChooseDescription::updateYetLen(const QString& text) {
	l_yetlen->setText(" "+QString::number(GG_STATUS_DESCR_MAXSIZE - text.length()));
}

IconsManager::IconsManager(const QString& name, const QString& configname)
	:Themes(name, configname)
{
    connect(this, SIGNAL(themeChanged(const QString&)), this, SLOT(changed(const QString&)));
}

QString IconsManager::iconPath(const QString &name)
{
	QString fname;	
	if(name.contains('/'))
		fname = name;
	else
		fname = themePath() + getThemeEntry(name);
	return fname;
}

QPixmap IconsManager::loadIcon(const QString &name)
{
	for (unsigned int i = 0; i < icons.count(); i++)
		if (icons[i].name == name)
			return icons[i].picture.pixmap();
	iconhandle icon;
	icon.name = name;
	QPixmap p;
	p.load(iconPath(name));
	icon.picture = QIconSet(p);
	icons.append(icon);
	return icons[icons.count()-1].picture.pixmap();
}

void IconsManager::onDestroyConfigDialog()
{
	kdebugf();
	QComboBox *cb_icontheme= ConfigDialog::getComboBox("Look", "Icon theme");
	QString theme;
	if (cb_icontheme->currentText() == tr("Default"))
		theme= "default";
	else
	    theme= cb_icontheme->currentText();

	config_file.writeEntry("Look", "IconsPaths", icons_manager.additionalPaths().join(";"));
	config_file.writeEntry("Look", "IconTheme", theme);
}

void IconsManager::chooseIconTheme(const QString& string)
{
	kdebugf();
	QString str=string;
	if (string == tr("Default"))
	    str= "default";
	icons_manager.setTheme(str);
	QMessageBox::information(0, tr("Icons"), tr("Please restart kadu to apply new icon theme"));
}

void IconsManager::onCreateConfigDialog()
{
	kdebugf();
	QComboBox *cb_icontheme= ConfigDialog::getComboBox("Look", "Icon theme");
	cb_icontheme->insertStringList(icons_manager.themes());
	cb_icontheme->setCurrentText(config_file.readEntry("Look", "IconTheme"));
	if (icons_manager.themes().contains("default"))
	cb_icontheme->changeItem(tr("Default"), icons_manager.themes().findIndex("default"));

	SelectPaths *selpaths= ConfigDialog::getSelectPaths("Look", "Icon paths");
	QStringList pl(QStringList::split(";", config_file.readEntry("Look", "IconsPaths")));
	selpaths->setPathList(pl);	
}

void IconsManager::initModule()
{
	kdebugf();
	config_file.addVariable("Look", "IconsPaths", "");
	config_file.addVariable("Look", "IconTheme", "default");
	
	icons_manager.setPaths(QStringList::split(";", config_file.readEntry("Look", "IconsPaths")));
	icons_manager.setTheme(config_file.readEntry("Look","IconTheme"));

	ConfigDialog::addTab("General");
	ConfigDialog::addTab("ShortCuts");
	ConfigDialog::addTab("Chat");
	ConfigDialog::addTab("Look");
	ConfigDialog::addHBox("Look", "Look", "icon_theme");
	ConfigDialog::addComboBox("Look", "icon_theme", QT_TRANSLATE_NOOP("@default","Icon theme"));
	ConfigDialog::addSelectPaths("Look", "icon_theme", QT_TRANSLATE_NOOP("@default","Icon paths"));
	
	ConfigDialog::registerSlotOnCreate(&icons_manager, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(&icons_manager, SLOT(onDestroyConfigDialog()));
	ConfigDialog::connectSlot("Look", "Icon theme", SIGNAL(activated(const QString&)), &icons_manager, SLOT(chooseIconTheme(const QString&)));
	ConfigDialog::connectSlot("Look", "Icon paths", SIGNAL(changed(const QStringList&)), &icons_manager, SLOT(selectedPaths(const QStringList&)));

}

void IconsManager::selectedPaths(const QStringList& paths)
{
	icons_manager.setPaths(paths);
	QComboBox* cb_icontheme= ConfigDialog::getComboBox("Look","Icon theme");
	QString current= cb_icontheme->currentText();
	
	cb_icontheme->clear();
	cb_icontheme->insertStringList(icons_manager.themes());
	cb_icontheme->setCurrentText(current);

	if (paths.contains("default"))
	cb_icontheme->changeItem(tr("Default"), paths.findIndex("default"));
}

void IconsManager::changed(const QString& theme)
{
//    icons.clear();
}

IconsManager icons_manager("icons", "icons.conf");
/********** HttpClient **********/

HttpClient::HttpClient()
{
	connect(&Socket,SIGNAL(connected()),this,SLOT(onConnected()));
	connect(&Socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
	connect(&Socket,SIGNAL(connectionClosed()),this,SLOT(onConnectionClosed()));
}

void HttpClient::onConnected()
{
	QString query = (PostData.size() > 0 ? "POST" : "GET");
	query += " ";
	
	if(Path.left(7)!="http://" && config_file.readBoolEntry("Network", "UseProxy"))
		query += "http://" + Host;		

	if ((Path == "" || Path[0] != '/') && Path.left(7)!="http://")
		query += '/';
		

	query += Path;
	query += " HTTP/1.1\r\n";
	query += "Host: " + Host + "\r\n";
	query += "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.4) Gecko/20030617\r\n";
//	query += "Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8\r\n";
//	query += "Connection: keep-alive\r\n";
	if(Referer!="")
		query += "Referer: "+Referer+"\r\n";
	if (Cookies.size() > 0) {
		query += "Cookie: ";

		QValueList<QString> keys;
		for (QMap<QString, QString>::const_iterator it = Cookies.begin(); it != Cookies.end(); ++it)
			keys.append(it.key());
		
//		wywolanie Cookies.keys() zostaje na lepsze czasu jak juz
//		wszyscy beda mieli Qt >= 3.0.5		
//    		for(int i=0; i<Cookies.keys().size(); i++)
		for (unsigned int i = 0; i < keys.size(); i++) {
			if (i > 0)
				query+="; ";
//			query+=Cookies.keys()[i]+"="+Cookies[Cookies.keys()[i]];
			query += keys[i] + "=" + Cookies[keys[i]];
			}
		query+="\r\n";
		}
	if (PostData.size() > 0) {
		query += "Content-Type: application/x-www-form-urlencoded\r\n";
		query += "Content-Length: " + QString::number(PostData.size()) + "\r\n";
		}
	query+="\r\n";
	if (PostData.size() > 0)
		query += QString(PostData);
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Sending query:\n%s\n", query.local8Bit().data());
	Socket.writeBlock(query.local8Bit().data(), query.length());
}

void HttpClient::onReadyRead()
{
	int size=Socket.bytesAvailable();
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Data Block Retreived: %i bytes\n",size);
	// Dodaj nowe dane do starych
	char buf[size];
	Socket.readBlock(buf,size);
	//
//	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "%s\n",buf);
	//
	int old_size=Data.size();
	Data.resize(old_size+size);
	for(int i=0; i<size; i++)
		Data[old_size+i]=buf[i];
	// Jesli nie mamy jeszcze naglowka
	if(!HeaderParsed)
	{	
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Trying to parse header\n");
		// Kontynuuj odczyt jesli naglowek niekompletny
		QString s=QString(Data);
		int p=s.find("\r\n\r\n");
		if(p<0)
			return;
		// Dostalismy naglowek, 
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Http header found:\n%s\n",s.local8Bit().data());		
		HeaderParsed=true;
		// Wyci±gamy status
		QRegExp status_regexp("HTTP/1\\.[01] (\\d+)");
		if(status_regexp.search(s)<0)
		{
			Socket.close();
			emit error();
			return;
		}
		Status=status_regexp.cap(1).toInt();
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Status: %i\n",Status);
		// Status 302 oznacza przekierowanie.
		if(Status==302)
		{
			QRegExp location_regexp("Location: ([^\\r\\n]+)");
			if(location_regexp.search(s)<0)
			{
				Socket.close();
				emit error();
				return;
			}
			QString location=location_regexp.cap(1);
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Jumping to %s\n",location.local8Bit().data());
			// czekamy. zbyt szybkie przekierowanie konczy sie
			// czasem petla. nie wiem dlaczego.
			QTime* t=new QTime();
			t->start();
			while (t->elapsed()<500) {};
			delete t;
			//
			emit redirected(location);
			get(location);
			return;
		}
		// Wyci±gamy Content-Length
		QRegExp cl_regexp("Content-Length: (\\d+)");
		ContentLengthNotFound=cl_regexp.search(s)<0;
		if(ContentLengthNotFound)
		{
			ContentLength=0;
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Content-Length not found. We will wait for connection to close.");
		}
		else
		{
			ContentLength=cl_regexp.cap(1).toUInt();
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Content-Length: %i bytes\n",ContentLength);			
		}
		
		// Wyciagamy ewentualne cookie (dla uproszczenia tylko jedno)
		QRegExp cookie_regexp("Set-Cookie: ([^=]+)=([^;]+);");
		if(cookie_regexp.search(s)>=0)
		{
			QString cookie_name=cookie_regexp.cap(1);
			QString cookie_val=cookie_regexp.cap(2);
			Cookies.insert(cookie_name,cookie_val);
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Cookie retreived: %s=%s\n",cookie_name.local8Bit().data(),cookie_val.local8Bit().data());
		}
		// Wytnij naglowek z Data
		int header_size=p+4;
		int new_data_size=Data.size()-header_size;
		for(int i=0; i<new_data_size; i++)
			Data[i]=Data[header_size+i];
		Data.resize(new_data_size);
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Header parsed and cutted off from data\n");
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: Header size: %i bytes\n",header_size);
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: New data block size: %i bytes\n",new_data_size);
		// Je¶li status jest 100 - Continue to czekamy na dalsze dane
		// (uniewa¿niamy ten nag³owek i czekamy na nastêpny)
		if(Status==100)
		{
			HeaderParsed=false;
			return;
		}
	}
	// Kontynuuj odczyt jesli dane niekompletne
	// lub je¶li mamy czekaæ na connection close
	if(ContentLength>Data.size()||ContentLengthNotFound)
		return;
	// Mamy cale dane
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "HttpClient: All Data Retreived: %i bytes\n",Data.size());
	Socket.close();
	emit finished();
}

void HttpClient::onConnectionClosed()
{
	if(HeaderParsed&&ContentLengthNotFound)
		emit finished();
	else
		emit error();
}

void HttpClient::setHost(QString host)
{
	Host=host;
	Cookies.clear();
}

void HttpClient::get(QString path)
{
	Referer=Path;
	Path=path;
	Data.resize(0);
	PostData.resize(0);
	HeaderParsed=false;
	if(config_file.readBoolEntry("Network", "UseProxy", false))
		Socket.connectToHost(
			config_file.readEntry("Network", "ProxyHost"),
			config_file.readNumEntry("Network", "ProxyPort"));
	else
		Socket.connectToHost(Host,80);
}

void HttpClient::post(QString path,const QByteArray& data)
{
	Referer=Path;
	Path=path;
	Data.resize(0);
	PostData.duplicate(data);
	HeaderParsed=false;
	Socket.connectToHost(Host,80);
}

void HttpClient::post(QString path,const QString& data)
{
	QByteArray PostData;
	PostData.duplicate(data.local8Bit().data(),data.length());
	post(path,PostData);
}

int HttpClient::status()
{
	return Status;
}

const QByteArray& HttpClient::data()
{
	return Data;
}

QString HttpClient::encode(const QString& text)
{
	QString encoded=text;
	QUrl::encode(encoded);
	return encoded;
}

void HtmlDocument::escapeText(QString& text)
{
	text.replace(QRegExp("&"), "&amp;");
	text.replace(QRegExp("<"), "&lt;");
	text.replace(QRegExp(">"), "&gt;");
	text.replace(QRegExp("\""), "&quot;");
	text.replace(QRegExp("  "), "&nbsp; ");
}

void HtmlDocument::unescapeText(QString& text)
{
	text.replace(QRegExp("&nbsp;"), " ");
	text.replace(QRegExp("&amp;"), "&");
	text.replace(QRegExp("&lt;"), "<");
	text.replace(QRegExp("&gt;"), ">");
	text.replace(QRegExp("&quot;"), "\"");
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
	e.tag=false;
	for(unsigned int i=0; i<html.length(); i++)
	{
		QChar ch=html[i];
		switch(ch)
		{
			case '<':
				if(!e.tag)
				{
					if(e.text!="")
						addElement(e);
					e.tag=true;
					e.text="<";
				}
				break;
			case '>':
				if(e.tag)
				{
					e.text+='>';
					addElement(e);
					e.tag=false;
					e.text="";
				}
				break;
			default:
				e.text+=ch;
		}
	}
	if(e.text!="")
		addElement(e);
}

QString HtmlDocument::generateHtml()
{
	QString html;
	for(unsigned int i=0; i<Elements.size(); i++)
	{
		Element e=Elements[i];
		if(!e.tag)
			escapeText(e.text);
		html+=e.text;
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

QString HtmlDocument::elementText(int index) const
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
		index++;
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
	QRegExp url_regexp("(http://|https://|www\\.|ftp://|ftp\\.|sftp://|smb://|file:/|rsync://|mailto:)[a-zA-Z0-9\\-\\._/~?=&#\\+%:;,!@\\\\]+");
	for(int i = 0; i < countElements(); i++)
	{
		if(isTagElement(i))
			continue;
		QString text=elementText(i);
		int p=url_regexp.search(text);
		if (p < 0)
			continue;
		int l=url_regexp.matchedLength();
		QString link="<a href=\""+text.mid(p,l)+"\">"+text.mid(p,l)+"</a>";
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

	for (it = 0, pos = 0, formats_length = 0, inspan = -1; it < htmldoc.countElements(); it++) {
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
				for (i = 0; i < attribs.count(); i++) {
					actattrib.name = attribs[i].section(":", 0, 0);
					actattrib.value = attribs[i].section(":", 1, 1);
					formantattribs.append(actattrib);
					}
				actformant.format.position = pos;
				actformant.format.font = 0;
				for (i = 0; i < formantattribs.count(); i++) {
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
		for (QValueList<struct richtext_formant>::iterator it = formants.begin(); it != formants.end(); it++) {
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

void ImageWidget::paintEvent(QPaintEvent *e)
{
	if (!Image.isNull()) {
		QPainter p(this);
		p.drawImage(0,0,Image);
	}
}

token::token() : QObject() {
	h = NULL;
	snr = snw = NULL;
}

token::~token() {
	deleteSocketNotifiers();
	if (h) {
		gg_token_free(h);
		h = NULL;
	}
}

void token::getToken() {
	kdebugf();
	if (!(h = gg_token(1))) {
		emit tokenError();
		return;
	}
	createSocketNotifiers();
}

void token::createSocketNotifiers() {
	kdebugf();

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, qApp->mainWidget());
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, qApp->mainWidget());
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void token::deleteSocketNotifiers() {
	kdebugf();
	if (snr) {
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
	}
	if (snw) {
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
	}
}

void token::dataReceived() {
	kdebugf();
	if (h->check && GG_CHECK_READ)
		socketEvent();
}

void token::dataSent() {
	kdebugf();
	snw->setEnabled(false);
	if (h->check && GG_CHECK_WRITE)
		socketEvent();
}

void token::socketEvent() {
	kdebugf();
	if (gg_token_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		emit tokenError();
		gg_token_free(h);
		h = NULL;
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "token::socketEvent(): getting token error\n");
		return;
	}

	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Register::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			emit tokenError();
			gg_token_free(h);
			h = NULL;
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "token::socketEvent(): getting token error\n");
			break;
		case GG_STATE_DONE:
			deleteSocketNotifiers();
			if (p->success) {
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "token::socketEvent(): success\n");
				emit gotToken(h);
			}
			else {
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "token::socketEvent(): getting token error\n");
				emit tokenError();
			}
			gg_token_free(h);
			h = NULL;
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
	}
}

TokenDialog::TokenDialog(QDialog *parent, const char *name)
	: QDialog(parent, name) {
	QGridLayout *grid = new QGridLayout(this, 3, 2, 6, 5);

	QLabel *l_tokenimage = new QLabel(tr("Read this code ..."), this);
	tokenimage = new ImageWidget(this);

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

	connect(&Token, SIGNAL(gotToken(struct gg_http *)), this, SLOT(gotTokenReceived(struct gg_http *)));
	connect(&Token, SIGNAL(tokenError()), this, SLOT(tokenErrorReceived()));
	Token.getToken();
	show();
	b_cancel->setDefault(false);
	b_ok->setDefault(true);
	setEnabled(false);
}

void TokenDialog::getToken(QString &Tokenid, QString &Tokenval) {
	Tokenid = tokenid;
	Tokenval = tokenedit->text();
}

void TokenDialog::gotTokenReceived(struct gg_http *h) {
	kdebugf();
	struct gg_token *t = (struct gg_token *)h->data;
	tokenid = cp2unicode((unsigned char *)t->tokenid);

	//nie optymalizowac!!!
	QByteArray buf(h->body_size);
	for (unsigned int i = 0; i < h->body_size; i++)
		buf[i] = h->body[i];

	tokenimage->setImage(buf);
	setEnabled(true);
	tokenedit->setFocus();
}

void TokenDialog::tokenErrorReceived() {
	kdebugf();
	setEnabled(true);
	done(-1);
}

Themes::Themes(const QString& name, const QString& configname)
{
	Name= name;
	ConfigName= configname;
	ActualTheme="Custom";
}

QStringList Themes::getSubDirs(const QString& path)
{
	QDir dir(path);
	dir.setFilter(QDir::Dirs);
	QStringList subdirs, dirs=dir.entryList();
	dirs.remove(".");
	dirs.remove("..");
	for (QStringList::Iterator it= dirs.begin(); it!=dirs.end(); it++)
		{
		QFile s(path+"/"+(*it)+"/"+ConfigName);
		if (s.exists())
			subdirs.append((*it));
		}
	return subdirs;
}

const QStringList Themes::themes()
{
	return ThemesList;
}

void Themes::setTheme(const QString& theme)
{
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
	kdebugm(KDEBUG_INFO, "Theme: %s\n", ActualTheme.local8Bit().data());
}

QString Themes::theme()
{
	return ActualTheme;
}

QString Themes::fixFileName(const QString& path,const QString& fn)
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
	ThemesList.clear();
	ThemesPaths.clear();
	additional.clear();
	QStringList add, temp=paths+defaultKaduPathsWithThemes();
	QFile s;
	for (QStringList::Iterator it= temp.begin(); it!=temp.end(); it++)
		{
		s.setName((*it)+"/"+ConfigName);
		if (s.exists())
			{
			if (paths.findIndex(*it)!=-1)
			    additional.append(*it);

			ThemesPaths.append(*it);
			ThemesList.append((*it).section("/", -2));
			}
		}
	emit pathsChanged(ThemesPaths);
}

QStringList Themes::defaultKaduPathsWithThemes()
{
	QStringList default1, default2;
	default1=getSubDirs(dataPath("kadu/themes/"+Name));
	default2=getSubDirs(ggPath(Name));

	for (QStringList::Iterator it= default1.begin(); it!=default1.end(); it++)
		(*it)=dataPath("kadu/themes/"+Name+"/"+(*it)+"/");

	for (QStringList::Iterator it= default2.begin(); it!=default2.end(); it++)
		(*it)=ggPath(Name)+"/"+(*it)+"/";

	return default1+default2;    
}

QStringList Themes::paths()
{
    return ThemesPaths;
}
QStringList Themes::additionalPaths()
{
    return additional;
}

QString Themes::themePath(const QString& theme)
{
	QString t=theme;
	if (theme == "")
		t= ActualTheme;
	if (theme == "Custom")
		return "";    
	if (ThemesPaths.isEmpty())
		return "Custom";
	return ThemesPaths.grep(t).first();
}

QString Themes::getThemeEntry(const QString& name)
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

QString GaduImagesManager::imageHtml(const QString& file_name)
{
	return QString("<img src=\"%1\"/>").arg(file_name);
}

QString GaduImagesManager::loadingImageHtml(UinType uin,uint32_t size,uint32_t crc32)
{
	return QString("<img src=\"%1\" gg_sender=\"%2\" gg_size=\"%3\" gg_crc=\"%4\"/>")
		.arg(icons_manager.iconPath("LoadingImage")).arg(uin).arg(size).arg(crc32);
}

void GaduImagesManager::addImageToSend(const QString& file_name,uint32_t& size,uint32_t& crc32)
{
	kdebugf();
	ImageToSend img;
	QFile f(file_name);
	kdebugm(KDEBUG_INFO, "Opening file \"%s\"\n",file_name.local8Bit().data());
	if(!f.open(IO_ReadOnly))
	{
		kdebugm(KDEBUG_ERROR, "Error opening file\n");
		return;
	}
	img.size = f.size();
	img.file_name=file_name;
	img.data = new char[img.size];
	kdebugm(KDEBUG_INFO, "Reading file\n");
	f.readBlock(img.data,img.size);
	img.crc32 = gg_crc32(0,(const unsigned char*)img.data,img.size);
	kdebugm(KDEBUG_INFO, "Inserting into images to send: filename=%s, size=%i, crc32=%i\n\n",img.file_name.local8Bit().data(),img.size,img.crc32);
	ImagesToSend.append(img);
	size = img.size;
	crc32 = img.crc32;
}

void GaduImagesManager::sendImage(UinType uin,uint32_t size,uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching images to send: size=%u, crc32=%u\n",size,crc32);
	for(QValueList<ImageToSend>::Iterator i=ImagesToSend.begin(); i!=ImagesToSend.end(); i++)
	{
		if ((*i).size==size && (*i).crc32==crc32)
		{
			kdebugm(KDEBUG_INFO, "Image data found\n");
			gadu->sendImage(uin,(*i).file_name,(*i).size,(*i).data);
			delete[] (*i).data;
			kdebugm(KDEBUG_INFO, "Removing from images queue\n");	
			ImagesToSend.remove(i);
			return;
		}
	}
	kdebugm(KDEBUG_WARNING, "Image data not found\n");
}

QString GaduImagesManager::saveImage(UinType sender,uint32_t size,uint32_t crc32,const QString& filename,const char* data)
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
	img.file_name = path+"/"+file_name;
	QFile f(img.file_name);
	f.open(IO_WriteOnly);
	f.writeBlock(data,size);
	f.close();
	SavedImages.append(img);
	kdebugf2();
	return img.file_name;
}	

QString GaduImagesManager::getImageToSendFileName(uint32_t size,uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching images to send: size=%u, crc32=%u\n",size,crc32);
	for(QValueList<ImageToSend>::Iterator i=ImagesToSend.begin(); i!=ImagesToSend.end(); i++)
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

QString GaduImagesManager::getSavedImageFileName(uint32_t size,uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching saved images: size=%u, crc32=%u\n",size,crc32);
	for(QValueList<SavedImage>::Iterator i=SavedImages.begin(); i!=SavedImages.end(); i++)
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
	QString image_string = QString("<img src=\"%1\"/>").arg(getSavedImageFileName(size,crc32));
	QString new_text = text;
	int pos;
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
		setText(tr("This is not an image"));
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
	setFilter(tr("Images")+" (*.png *.PNG *.jpg *.JPG *.gif *.GIF *.bmp *.BMP)");
	setContentsPreviewEnabled(true);
	setContentsPreview(pp, pp);
	setPreviewMode(QFileDialog::Contents);
}

KaduTextBrowser::KaduTextBrowser(QWidget *parent, const char *name)
	: QTextBrowser(parent, name), level(0)
{
	connect(this, SIGNAL(linkClicked(const QString&)), this, SLOT(hyperlinkClicked(const QString&)));
	setWrapPolicy(QTextEdit::AtWordOrDocumentBoundary);
}

void KaduTextBrowser::setSource(const QString &name)
{
}

void KaduTextBrowser::copyLinkLocation()
{
	kdebugm(KDEBUG_FUNCTION_START, "KaduTextBrowser::copyLinkLocation(): anchor = %s\n", anchor.local8Bit().data());
	QApplication::clipboard()->setText(anchor);
}

QPopupMenu *KaduTextBrowser::createPopupMenu(const QPoint &point)
{
	kdebugf();
	anchor = anchorAt(point);
	QPopupMenu* popupmenu = QTextBrowser::createPopupMenu(point);

	if (!anchor.isEmpty())
		popupmenu->insertItem(tr("Copy link &location"), this, SLOT(copyLinkLocation()), CTRL+Key_L, -1, 0);

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
//	kdebugm(KDEBUG_INFO, "KaduTextBrowser::drawContents(): level: %d\n", level);
	level++;
	if (level==1)
		QTextBrowser::drawContents(p, clipx, clipy, clipw, cliph);
	level--;
}

void KaduTextBrowser::hyperlinkClicked(const QString &link)
{
	openWebBrowser(link);
}
