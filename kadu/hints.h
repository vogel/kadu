#ifndef HINTS_H
#define HINTS_H

#include "userlist.h"

#include <qlabel.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qpushbutton.h>

class Hint : public QHBoxLayout
{
 Q_OBJECT

	private:
		QLabel *icon;
		QLabel *label;
		QColor bcolor;
		unsigned int secs;
		unsigned int ident;
		uin_t uin;
		void enter();
		void leave();

	public:
		Hint(QWidget *parent, const QString &text, const QPixmap &pixmap, unsigned int timeout);
		~Hint();
		void setId(unsigned int id) {ident = id;}
		void setUin(uin_t i) { uin=i; };
		uin_t getUin() { return uin ; };
		unsigned int id() const {return ident;}

	protected:
		bool eventFilter(QObject *obj, QEvent *ev);		

	public slots:
		bool nextSecond();
		void setShown(bool);
		void set(const QFont &, const QColor &, const QColor &, unsigned int, bool show = true);

	signals:
		void leftButtonClicked(unsigned int);
		void rightButtonClicked();
		void midButtonClicked(unsigned int);
};

class HintManager : public QFrame
{
	Q_OBJECT

	private:
		QTimer *hint_timer;
		QGridLayout *grid;
		QPtrList<Hint> hints;
		bool useposition;
		QPoint position;
		QValueList<QStringList> config;

	public:
		HintManager();
		static void initModule();
		void addHint(const QString &, const QPixmap &, const QFont &, const QColor &, const QColor &, unsigned int, uin_t=0);
		void addHintError(const QString &);
		void addHintNewMsg(const QString &, const QString &);
		void addHintNewChat(const QString &, const QString &);
		void addHintStatus(const UserListElement &, unsigned int status, unsigned int oldstatus);

	public slots:
		void loadConfig();
		void deleteHint(unsigned int);

	private slots:
		void oneSecond();
		void setHint();
		void leftButtonSlot(unsigned int);
		void rightButtonSlot();
		void midButtonSlot(unsigned int);
};

class HintManagerSlots: public QObject
{
	Q_OBJECT

	private:
		QComboBox *combobox2;
		QComboBox *cb_notify;
		QSpinBox *sb_timeout;
		QSpinBox *sb_citesign;
		QLabel *preview;
		QLineEdit *e_posx;
		QLineEdit *e_posy;
		QCheckBox *b_useposition;
		QCheckBox *b_hint;
		QValueList<QStringList> hint;
		int newhintunder;
		bool useposition;
		QPoint hintsposition;

	private slots:
		void activatedChanged(int);
		void changeFontColor();
		void changeBackgroundColor();
		void changeFont();
		void changeTimeout(int);

	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
};

extern HintManager *hintmanager;

#endif
