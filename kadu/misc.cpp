
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

#include "misc.h"
#include "pixmaps.h"
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

void cp_to_iso(unsigned char *buf)
{
	while (*buf)
	{
		if (*buf == (unsigned char)'¥') *buf = '¡';
		if (*buf == (unsigned char)'¹') *buf = '±';
		if (*buf == 140) *buf = '¦';
		if (*buf == 156) *buf = '¶';
		if (*buf == 143) *buf = '¬';
		if (*buf == 159) *buf = '¼';
        	buf++;
        };
};

void iso_to_cp(unsigned char *buf)
{
	while (*buf)
	{
		if (*buf == (unsigned char)'¡') *buf = '¥';
		if (*buf == (unsigned char)'±') *buf = '¹';
		if (*buf == (unsigned char)'¦') *buf = 140;
		if (*buf == (unsigned char)'¶') *buf = 156;
		if (*buf == (unsigned char)'¬') *buf = 143;
		if (*buf == (unsigned char)'¼') *buf = 159;
		buf++;
	};
};

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

	desc = new QLineEdit(this);
	desc->setMaxLength(40);

	QPushButton *okbtn;
	okbtn = new QPushButton(this);
	okbtn->setText(i18n("&OK"));
	QPushButton *cancelbtn;
	cancelbtn = new QPushButton(this);
	cancelbtn->setText(i18n("&Cancel"));

	char **gg_xpm;
	switch (nr) {
		case 1:
			gg_xpm = (char **)gg_actdescr_xpm;
			break;
		case 3:
			gg_xpm = (char **)gg_busydescr_xpm;
			break;
		case 5:
			gg_xpm = (char **)gg_invidescr_xpm;
			break;
		case 7:
			gg_xpm = (char **)gg_inactdescr_xpm;
			break;
		default:
			gg_xpm = (char **)gg_inactdescr_xpm;
		}
	okbtn->setIconSet(QIconSet(QPixmap((const char**)gg_xpm)));

	QObject::connect(okbtn, SIGNAL(clicked()), this, SLOT(okbtnPressed()));
	QObject::connect(cancelbtn, SIGNAL(clicked()), this, SLOT(cancelbtnPressed()));

	QGridLayout *grid = new QGridLayout(this, 2, 2);

	grid->addMultiCellWidget(desc, 0, 0, 0, 1);
	grid->addWidget(cancelbtn, 1, 1, Qt::AlignRight);
	grid->addWidget(okbtn, 1, 0, Qt::AlignRight);
	grid->addColSpacing(0, 200);

	resize(250,80);
	desc->setText(own_description);
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

