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

#include "misc.h"

class Hint : public QHBoxLayout
{
 Q_OBJECT

	private:
		QLabel *icon;
		QLabel *label;
		QColor bcolor;
		unsigned int secs;
		unsigned int ident;
		UinsList uins;
		void enter();
		void leave();

	public:
		Hint(QWidget *parent, const QString &text, const QPixmap &pixmap, unsigned int timeout);
		~Hint();
		void setId(unsigned int id) {ident = id;}
		void setUins(UinsList i) { uins=i; };
		UinsList getUins() { return uins ; };
		unsigned int id() const {return ident;}

	protected:
		bool eventFilter(QObject *obj, QEvent *ev);		

	public slots:
		bool nextSecond();
		void setShown(bool);
		void set(const QFont &, const QColor &, const QColor &, unsigned int, bool show = true);

	signals:
		void leftButtonClicked(unsigned int);
		void rightButtonClicked(unsigned int);
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
		QPoint DetectedPosition;
		QValueList<QStringList> config;

	public:
		HintManager();
		static void initModule();
		void addHint(const QString &, const QPixmap &, const QFont &, const QColor &, const QColor &, unsigned int, UinsList* senders=0);
		void addHintError(const QString &);
		void addHintNewMsg(const QString &, const QString &);
		void addHintNewChat(UinsList& senders, const QString &);
		void addHintStatus(const UserListElement &, unsigned int status, unsigned int oldstatus);
		/**
			Ustawia automatycznie wykryt± pozycjê dla hintów.
			Mo¿e to zrobiæ np. modu³ docking.
		**/
		void setDetectedPosition(const QPoint& pos);

	public slots:
		void loadConfig();
		void deleteHint(unsigned int);
		void openChat(unsigned int id);
		void deleteAllHints();

	private slots:
		void oneSecond();
		void setHint();
		void leftButtonSlot(unsigned int);
		void rightButtonSlot(unsigned int);
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
