#ifndef MISC_H
#define MISC_H

#include <qvaluelist.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qcstring.h>
#include <libintl.h>

#include "libgadu.h"

#define __c2q(__char_pointer__) QString::fromLocal8Bit(__char_pointer__)

/*
	Zmienia sciezke relatywna do katalogu z ustawieniami gg
	na sciezke bezwzgledna uwzgledniajac zmienne srodowiskowe
	$HOME i $CONFIG_DIR
*/
QString ggPath(QString subpath);

#define i18n(String) __c2q(gettext(String))

QString cp_to_iso(unsigned char *);
QCString iso_to_cp(const QString &);
QPixmap loadIcon(const QString &filename);
char *timestamp(time_t = 0);
QString pwHash(const QString tekst);
void deleteSearchIdStruct(QDialog *ptr);

enum {
	DIALOG_SEARCH,
	DIALOG_PERSONAL
};

struct SearchIdStruct {
	QDialog *ptr;
	uint32_t seq;
	int type;
};

extern QValueList<struct SearchIdStruct> SearchList;

class UinsList : public QValueList<uin_t>
{
	public:
		UinsList();
		bool equals(UinsList &uins);
		void sort();
};

class ChooseDescription : public QDialog {
	Q_OBJECT
	public:
		ChooseDescription ( int nr, QWidget * parent=0, const char * name=0);

	private:
		QLineEdit * desc;
		QLabel *l_yetlen;

	private slots:
		void okbtnPressed();
		void cancelbtnPressed();
		void updateYetLen(const QString&);

};

struct iconhandle {
	QString name;
	QPixmap pixmap;
};

class IconsManager {
	public:
		IconsManager();
		IconsManager(QString &dir);
		void setDirectory(QString &dir);
		QPixmap *loadIcon(QString name);
		void clear();

	private:
		QValueList<iconhandle> icons;
		QString directory;
};

extern IconsManager *icons;

#endif
