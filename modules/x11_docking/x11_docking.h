#ifndef X11_DOCKING_H
#define X11_DOCKING_H

#include <qlabel.h>
#include <qpixmap.h>

class X11TrayIcon : public QLabel
{
	Q_OBJECT

	private slots:
		void setTrayPixmap(const QPixmap& pixmap);
		void setTrayTooltip(const QString& tooltip);
		void findTrayPosition(QPoint& pos);

	protected:
		virtual void enterEvent(QEvent* e);
		virtual void mousePressEvent(QMouseEvent* e);

	public slots:
		void disableTaskbar();
		void enableTaskbar(bool enable=true);

	public:
		X11TrayIcon();
		~X11TrayIcon();
		void show();
};

extern X11TrayIcon* x11_tray_icon;

#endif
