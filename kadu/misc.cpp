#include <qpushbutton.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qcstring.h>
#include <qdatetime.h>
#include <qregexp.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "misc.h"
#include "pixmaps.h"
#include "config_dialog.h"
#include "kadu.h"
#include "debug.h"

QValueList<struct SearchIdStruct> SearchList;

QString ggPath(QString subpath)
{
	QString path;
	char *home;
	struct passwd *pw;
	if (pw = getpwuid(getuid()))
		home = pw->pw_dir;
	else
		home = getenv("HOME");
	char *config_dir = getenv("CONFIG_DIR");
	if (config_dir == NULL)
		path = QString("%1/.gg/%2").arg(home).arg(subpath);
	else
		path = QString("%1/%2/gg/%3").arg(home).arg(config_dir).arg(subpath);
	return path;
};

QString cp2unicode(unsigned char *buf)
{
	QTextCodec *codec = QTextCodec::codecForName("CP1250");
	if (buf)
		return codec->toUnicode((const char*)buf);
	else
		return QString::null;
}

QCString unicode2cp(const QString &buf)
{
	QTextCodec *codec = QTextCodec::codecForName("CP1250");
	return codec->fromUnicode(buf);
}

QPixmap loadIcon(const QString &filename) {
	QPixmap icon;
	
	icon.load(QString(DATADIR) + "/apps/kadu/icons/" + filename);
	return icon;
}

QString timestamp(time_t customtime)
{
	QString buf;
	QDateTime date;
	time_t t;
	
	t = time(NULL);

	date.setTime_t(t);
	buf.append(date.toString(":: dd.MM.yyyy (hh:mm:ss"));

	if (customtime) {
		date.setTime_t(customtime);
		buf.append(QString(" / S ") + date.toString("hh:mm:ss)"));
		}
	else
		buf.append(")");

	return buf;
}

QDateTime currentDateTime(void) {
	time_t t;
	QDateTime date;

	t = time(NULL);
	date.setTime_t(t);
	return date;
}

QString pwHash(const QString tekst) {
	QString nowytekst;
	int ile, znak;
	nowytekst = tekst;
	for (ile = 0; ile < tekst.length(); ile++) {
		znak = nowytekst[ile].latin1() ^ ile ^ 1;
		nowytekst[ile] = znak;
		}
	return nowytekst;
}

void escapeSpecialCharacters(QString &msg) {
	msg.replace(QRegExp("&"), "&amp;");
	msg.replace(QRegExp("<"), "&lt;");
	msg.replace(QRegExp(">"), "&gt;");				
}

QString formatGGMessage(const QString &msg, int formats_length, void *formats) {
	QString mesg, tmp;
	bool bold, italic, underline, color;
	char *cformats = (char *)formats;
	struct gg_msg_richtext_format *actformat;
	struct gg_msg_richtext_color *actcolor;
	int pos;

	kdebug("formatGGMessage()\n");
	bold = italic = underline = color = false;
	pos = 0;
	if (formats_length) {
		while (formats_length) {
			actformat = (struct gg_msg_richtext_format *)cformats;
			if (actformat->position > pos) {
				tmp = msg.mid(pos, actformat->position - pos);
				escapeSpecialCharacters(tmp);
				mesg.append(tmp);
				pos += actformat->position;
				}
			else {
				if (actformat->font & GG_FONT_BOLD) {
					if (!bold) {
						mesg.append("<B>");
						bold = true;
						}
					}
				else
					if (bold) {
						mesg.append("</B>");
						bold = false;
						}
				if (actformat->font & GG_FONT_ITALIC) {
					if (!italic) {
						mesg.append("<I>");
						italic = true;
						}
					}
				else
					if (italic) {
						mesg.append("</I>");
						italic = false;
						}
				if (actformat->font & GG_FONT_UNDERLINE) {
					if (!underline) {
						mesg.append("<U>");
						underline = true;
						}
					}
				else
					if (underline) {
						mesg.append("</U>");
						underline = false;
						}
				if (actformat->font & GG_FONT_COLOR) {
					actcolor = (struct gg_msg_richtext_color *)(cformats
						+ sizeof(struct gg_msg_richtext_format));
					mesg.append(QString("<FONT color=\"#%1%2%3\">").arg(actcolor->red, 0, 16)
						.arg(actcolor->green, 0, 16).arg(actcolor->blue, 0, 16));
					color = true;
					}
				else
					if (color) {
						mesg.append("</FONT>");
						color = false;
						}
				cformats += sizeof(gg_msg_richtext_format);
				formats_length -= sizeof(gg_msg_richtext_format);
				cformats += sizeof(gg_msg_richtext_color) * ((actformat->font & GG_FONT_COLOR) != 0);
				formats_length -= sizeof(gg_msg_richtext_color) * ((actformat->font & GG_FONT_COLOR) != 0);
				}
			}
		if (pos < msg.length()) {
			tmp = msg.mid(pos, msg.length() - pos);
			escapeSpecialCharacters(tmp);
			mesg.append(tmp);
			}
		}
	else {
		mesg = msg;
		escapeSpecialCharacters(mesg);
		}
	kdebug("formatGGMessage(): finished\n");
	return mesg;
}

QString parse_symbols(QString s, int i, UserListElement &ule, bool escape) {
	QString r,d;
	int j;

	kdebug("parse_symbols():%s, %d escape=%i\n",(const char *)s.local8Bit(),i,escape);

	while(s[i]!='%' && i != s.length()) {
		r+=s[i];
		i++;
	}

	if (s[i]=='%') {
		i++;
		switch(s[i].latin1()) {
			case 's':
				i++;
				if (!ule.uin)
					break;
				j=statusGGToStatusNr(ule.status);
				if (j == 1 || j == 3 || j == 5 || j == 7)
					r+=i18n(statustext[j-1]);
				else
					r+=i18n(statustext[j]);
				break;
			case 'd':
				i++;
				if (!escape)
					r+=ule.description;
				else {
					d=ule.description;
					d.replace(QRegExp("<"), "&lt;");
					d.replace(QRegExp(">"), "&gt;");
					r+=d;
				}
				break;
			case 'i':
				i++;
				if (ule.ip.ip4Addr())
					r += ule.ip.toString();
				break;
			case 'v':
				i++;
				if (ule.ip.ip4Addr())
					r+=ule.dnsname;
				break;
			case 'n':
				i++;
				r+=ule.nickname;
				break;
			case 'a':
				i++;
				r+=ule.altnick;
				break;
			case 'f':
				i++;
				r+=ule.first_name;
				break;
			case 'r':
				i++;
				r+=ule.last_name;
				break;
			case 'm':
				i++;
				r+=ule.mobile;
				break;
			case 'u':
				i++;
				if (ule.uin)
					r+=QString::number(ule.uin);
				break;
			case 'g':
				i++;
				r+=ule.group;
				break;
			case 'o':
				i++;
				if (ule.port==2)
					r+=" ";
				break;
			case 'p':
				i++;
				if (ule.port)
					r+=QString::number(ule.port);
				break;
		}
	}

	if (i != s.length())
		r+=parse_symbols(s,i,ule,escape);
	return r;
}

QString parse_only_text(QString s, int i) {
	QString r;

	kdebug("parse_only_text() %s, %d\n",(const char *)s.local8Bit(),i);

	while(s[i]!='%' && i != s.length()) {
	r+=s[i];
	i++;
	}

	if(s[i]=='%')
		i+=2;

	if(i!=s.length())
		r+=parse_only_text(s,i);
	return r;
}

QString parse_expression(QString s, int& i, UserListElement &ule, bool escape) {
	QString p,r,f;

	kdebug("parse_expression() %s, %d escape=%i\n",(const char *)s.local8Bit(),i,escape);

	while(s[i]!='[' && i != s.length()) {
		f+=s[i];
		i++;
	}

	r+=parse_symbols(f,0,ule,escape);

	if(s[i]=='['){
		i++;
		while(s[i]!=']' && i != s.length()) {
			p+=s[i];
			i++;
		}

		if(s[i]==']') {
			i++; //eat ]
			if(parse_only_text(p,0)!=parse_symbols(p,0,ule,escape))
				r+=parse_symbols(p,0,ule,escape);
			if(i == s.length())
				return r;
			else
				r+=parse_expression(s,i,ule,escape);
		}
	}
	
	if(i != s.length())
		r+=parse_expression(s,i,ule,escape);
	return r;
}

QString parse(QString s, UserListElement ule, bool escape) {
	int i=0;

	kdebug("parse() :%s escape=%i\n",(const char *)s.local8Bit(),escape);

	return parse_expression(s,i,ule,escape);
}

void deleteSearchIdStruct(QDialog *ptr) {
	int i = 0;
	while (i < SearchList.count() && SearchList[i].ptr != ptr)
		i++;
	if (i < SearchList.count())
		SearchList.remove(SearchList.at(i));
}

bool UinsList::equals(UinsList &uins) {
	if (count() != uins.count())
		return false;
	for (UinsList::iterator i = begin(); i != end(); i++)
		if(!uins.contains(*i))
			return false;
	return true;
}

UinsList::UinsList() {
}

void UinsList::sort() {
	bool stop;
	int i;
	uin_t uin;
	
	if (count() < 2)
		return;

	do {
		stop = true;
		for (i = 0; i < count() - 1; i++)
			if (this->operator[](i) > this->operator[](i+1)) {
				uin = this->operator[](i);
				this->operator[](i) = this->operator[](i+1);
				this->operator[](i+1) = uin;
				stop = false;
				}
	} while (!stop);	
}

ChooseDescription::ChooseDescription ( int nr, QWidget * parent, const char * name)
: QDialog(parent, name, true) {
	setWFlags(Qt::WDestructiveClose);
	setCaption(i18n("Select description"));

	desc = new QLineEdit(own_description,this);
	desc->setMaxLength(GG_STATUS_DESCR_MAXSIZE);

	l_yetlen = new QLabel(" "+QString::number(GG_STATUS_DESCR_MAXSIZE - desc->text().length()),this);
	connect(desc, SIGNAL(textChanged(const QString&)), this, SLOT(updateYetLen(const QString&)));

	QPixmap *pix;
	switch (nr) {
		case 1:
			pix = icons->loadIcon("online_d");
			break;
		case 3:
			pix = icons->loadIcon("busy_d");
			break;
		case 5:
			pix = icons->loadIcon("invisible_d");
			break;
		case 7:
			pix = icons->loadIcon("offline_d");
			break;
		default:
			pix = icons->loadIcon("offline_d");
		}

	QPushButton *okbtn = new QPushButton(QIconSet(*pix), i18n("&OK"), this);
	QPushButton *cancelbtn = new QPushButton(i18n("&Cancel"), this);
	

	QObject::connect(okbtn, SIGNAL(clicked()), this, SLOT(okbtnPressed()));
	QObject::connect(cancelbtn, SIGNAL(clicked()), this, SLOT(cancelbtnPressed()));

	QGridLayout *grid = new QGridLayout(this, 2, 2);

	grid->addMultiCellWidget(desc, 0, 0, 0, 2);
	grid->addWidget(l_yetlen, 1, 0);
	grid->addWidget(okbtn, 1, 1, Qt::AlignRight);
	grid->addWidget(cancelbtn, 1, 2, Qt::AlignRight);
	grid->addColSpacing(0, 200);

	resize(250,80);
	desc->selectAll();
}

void ChooseDescription::okbtnPressed() {
	own_description = desc->text();
	config.defaultdescription = own_description;
	accept();
}

void ChooseDescription::cancelbtnPressed() {
	reject();
//	close();
}

void ChooseDescription::updateYetLen(const QString& text) {
	l_yetlen->setText(" "+QString::number(GG_STATUS_DESCR_MAXSIZE - text.length()));
}

IconsManager::IconsManager() {
}

IconsManager::IconsManager(QString &dir) {
	directory = dir;
}

void IconsManager::setDirectory(QString &dir) {
	directory = dir;
}

QPixmap *IconsManager::loadIcon(QString name) {
	int i;
	QString fname;

	for (i = 0; i < icons.count(); i++)
		if (icons[i].name == name)
			break;
	if (i < icons.count()) {
		return &icons[i].pixmap;
		}
	else {
		iconhandle icon;
		icon.name = name;
		QPixmap p;
		fname = directory + "/";
		fname = fname + name + ".png";
		p.load(fname);
		icon.pixmap = p;
		icons.append(icon);
		return &icons[i].pixmap;
		}		
}

void IconsManager::clear() {
	icons.clear();
}

IconsManager *icons = NULL;
