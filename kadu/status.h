#ifndef STATUS_H
#define STATUS_H

#include <qtimer.h>
#include <qstringlist.h>
#define GG_STATUS_INVISIBLE2 0x0009 /* g³upy... */

extern QString gg_icons[];
extern int gg_statuses[];
extern const char *statustext[];
extern QString own_description;
extern QStringList defaultdescriptions;
bool ifStatusWithDescription(int status);
int getActualStatus();
int statusGGToStatusNr(int);

class AutoStatusTimer : public QTimer
{
	Q_OBJECT

	private slots:
		void onTimeout();

	public:
		AutoStatusTimer(QObject *parent = 0);

	private:
		int length_buffor;
};

class AutoAwayTimer : private QTimer
{
	Q_OBJECT

	private slots:
		void checkIdleTime();

	public:
		static void on();
		static void off();
		static void initModule();

	protected:
		bool eventFilter(QObject *, QEvent *);

	private:
		AutoAwayTimer(QObject *parent = 0);
		static AutoAwayTimer *autoaway_object;
		bool autoawayed;
		int beforeAutoAway;
		int idletime;
};

class AutoAwaySlots :public QObject
{
	Q_OBJECT
	
	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
	
};
#endif
