#ifndef DOCKING_H
#define DOCKING_H

#include <qlabel.h>
#include <qpixmap.h>

/**
	Klasa reprezentuj±ca ikonkê w obszarze powiadamiania
**/
class TrayIcon : public QLabel
{
	Q_OBJECT

	private:
		QWidget* WMakerMasterWidget;
		QTimer *icon_timer;
		bool blink;
		
	private slots:
		void pendingMessageAdded();
		void pendingMessageDeleted();
		void showOffline();
		void showStatus(int status);
		void showCurrentStatus(int status);

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
		void setType(const QPixmap& pixmap);
		void connectSignals();
		
	public slots:
		// Status change slots
		void dockletChange(int id);
		//Funkcja do migania koperty
		void changeIcon();

	signals:
		void mousePressMidButton();
		void mousePressLeftButton();
		void mousePressRightButton();
		
	public:
		static void initModule();
		static void closeModule();
};

extern TrayIcon *trayicon;

#endif
