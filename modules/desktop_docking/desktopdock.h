#ifndef DESKTOPDOCK_H
#define DESKTOPDOCK_H

#include <qpixmap.h>
#include <qlabel.h>

class DesktopDockWindow : public QLabel
{	
	Q_OBJECT

	public:
		DesktopDockWindow(QWidget *parent=0, const char *name=0);
		~DesktopDockWindow();
		virtual void mousePressEvent(QMouseEvent *ev);
		void updateMask();
	private slots:
		void setToolTip(const QString& statusText);
		void ApplyConfig();
		void onCreateConfigDialog();
		void setPixmap(const QPixmap& DockPixmap);
		void findTrayPosition(QPoint& DockPoint);
		void enableColorButton(bool b);
};

extern DesktopDockWindow *desktop_dock_window;
#endif
