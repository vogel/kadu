#ifndef DOCKAPP_H
#define DOCKAPP_H

#include <qlabel.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qpixmap.h>

/**
	Klasa reprezentuj±ca ikonkê w obszarze powiadamiania
**/
class TrayIcon : public QLabel
{
	Q_OBJECT

	private:
		friend class TrayHint;
		QWidget* WMakerMasterWidget;
		TrayHint *hint;
		QTimer *icon_timer;
		bool blink;

	protected:
		void setPixmap(const QPixmap& pixmap);
		virtual void resizeEvent(QResizeEvent* e);
		virtual void enterEvent(QEvent* e);
		virtual void mousePressEvent(QMouseEvent*);

	public:
		TrayIcon(QWidget *parent = 0, const char *name = 0);
		~TrayIcon();
		QPoint trayPosition();
		void show();
		void setType(QPixmap &pixmap);
		void connectSignals();
		void showHint(const QString&, const QString&, int index);
		void showErrorHint(const QString&);
		void reconfigHint();
		static void initModule();
		
	public slots:
		// Status change slots
		void dockletChange(int id);
		//Funkcja do migania koperty
		void changeIcon();

	signals:
		void mousePressMidButton();
		void mousePressLeftButton();
		void mousePressRightButton();
};

class TrayHint : public QWidget
{
	Q_OBJECT
	
	private:
		QTimer *hint_timer;
		QTextBrowser *hint;
		QStringList hint_list;

	public:
		TrayHint(QWidget *parent=0, const char *name = 0);
		void show_hint(const QString&, const QString&, int index);
		void restart();
		
	public slots:
		void remove_hint();

	private slots:
		void set_hint();
};

extern TrayIcon *trayicon;

class TraySlots: public QObject
{
	Q_OBJECT

	public slots:
		void onCreateConfigDialog();
};

#endif
