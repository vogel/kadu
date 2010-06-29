#ifndef KADU_TEXT_BROWSER_H
#define KADU_TEXT_BROWSER_H

#include <QtCore/QTimer>
#include <QtGui/QTextBrowser>
#include <QtWebKit/QWebView>

#include "exports.h"

/**
	Zmodyfikowany QTextBrowser specjalnie na potrzeby Kadu.
	Klikni�cie na linku otwiera ustawion� w konfiguracji przegl�dark�.
	\class KaduTextBrowser
	\brief Klasa bazowa dla ChatMessagesView oraz InfoPanel.
**/
class KADUAPI KaduTextBrowser : public QWebView //, private QToolTip
{
	Q_OBJECT

	QPoint contextMenuPos;
	QTimer refreshTimer; /*!< Timer od�wie�ania widgetu. */
	
private slots:

	/**
		\fn void refreshLater()
		Slot uruchamiaj�cy od�wie�enie z op��nieniem.
		@see refreshTimer
	**/
	void refreshLater();

	/**
		\fn void hyperlinkClicked(const QUrl &anchor) const
		Otwiera klikni�ty odno�nik w okre�lonej w konfiguracji przegl�darce.
		@param anchor Odno�nik do otwarcia.
	**/
	void hyperlinkClicked(const QUrl &anchor) const;
	void saveImage();

protected:
	virtual void contextMenuEvent(QContextMenuEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void wheelEvent(QWheelEvent *e);

public:
	KaduTextBrowser(QWidget *parent = 0);
	void setSource(const QString &name);
	void setMargin(int width);
	void setPage(QWebPage * page);

signals:
	/**
		Dowolny przycisk myszy zosta� zwolniony
		sender() - obiekt, kt�ry wygenerowa� zdarzenie
	**/
	void mouseReleased(QMouseEvent *e);

	/**
		U�yto rolki myszy
		sender() - obiekt, kt�ry wygenerowa� zdarzenie
	**/
	void wheel(QWheelEvent *e);

};

#endif
