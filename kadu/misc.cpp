
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qcstring.h>

#include "misc.h"
#include "pixmaps.h"
#include "config_dialog.h"
#include "kadu.h"

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

char *timestamp(time_t customtime)
{
	static char buf[100];

	time_t t;
	struct tm *tm;

	time(&t);

	tm = localtime(&t);
	strftime(buf, sizeof(buf), ":: %d %m %Y, (%T", tm);

	if (customtime) {
		char buf2[20];
		struct tm *tm2;
		tm2 = localtime(&customtime);
		strftime(buf2, sizeof(buf2), " / S %T)", tm2);
		strncat(buf, buf2, sizeof(buf2));
		
/*		int j = 0;
		while(buf[j++] != "\0");
		
		int i = -1;
		while(buf2[++i] != "\0") {
       buf[j+i] = buf2[i];
			}
		buf[j + ++i] = "\0"; */
	
		return buf;

		}

	strftime(buf, sizeof(buf), ":: %d %m %Y, (%T)", tm);

	return buf;
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
