#ifndef DESKTOPDOCK_H
#define DESKTOPDOCK_H

#include <qpixmap.h>
#include <qlabel.h>

/* DesktopDockWindow - ikonka dokujaca */

class DesktopDockWindow : public QLabel
{	
	Q_OBJECT

	public:
		DesktopDockWindow(QWidget *parent=0, const char *name=0);
		~DesktopDockWindow();
		void mousePressEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);
		void updateMask();
	private slots:
		void setToolTip(const QString &statusText);
		void ApplyConfig();
		void onCreateConfigDialog();
		void setPixmap(const QPixmap &DockPixmap);
		void findTrayPosition(QPoint &DockPoint);
		void enableColorButton(bool b);
		void droppedOnDesktop(const QPoint &);
		void startMoving();
		void updateMenu(bool);
	private:
		int menuPos;
		int separatorPos;
		bool isMoving;
};

extern DesktopDockWindow *desktop_dock_window;
#endif
