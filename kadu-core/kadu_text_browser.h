#ifndef KADU_TEXT_BROWSER_H
#define KADU_TEXT_BROWSER_H

#include <qfont.h>
#include <qpoint.h>
#include <qstring.h>
#include <qtextbrowser.h>
#include <qtimer.h>
#include <qtooltip.h>

class QPopupMenu;
class QPainter;
class QMouseEvent;

/**
	Zmodyfikowany QTextBrowser specjalnie na potrzeby Kadu.
	Klikniêcie na linku otwiera ustawion± w konfiguracji przegl±darkê.
	W menu kontekstowym jest dodatkowa opcja "Kopiuj lokacjê odno¶nika".
	Dodatkowo poprawka b³êdu w Qt.
**/
class KaduTextBrowser : public QTextBrowser, private QToolTip
{
	Q_OBJECT

	private:
		QTimer refreshTimer;
		QString anchor;
		int level;
		/**
			this value stores the current highlighted link
			for use with maybeTip(), or is null
		**/
		QString highlightedlink;
		QString image;
		bool trueTransparency;

	private slots:
		void refresh();
		void refreshLater();
		void copyLinkLocation();
		void hyperlinkClicked(const QString& link) const;
		void linkHighlighted(const QString &);
		void saveImage();
		void verticalSliderPressedSlot();
		void verticalSliderReleasedSlot();

	protected:
		QPopupMenu *createPopupMenu(const QPoint &point);
		virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph);
		virtual void maybeTip(const QPoint&);
		virtual void contentsMouseReleaseEvent(QMouseEvent * e);
		virtual void contentsWheelEvent(QWheelEvent * e);

	public:
		KaduTextBrowser(QWidget *parent = 0, const char *name = 0);
		void setSource(const QString &name);
		/**
			Nadpisane dla wyja¶nienia wieloznaczno¶ci
		**/
		void clear()
		{
			setText("");
			//dla QT<3.2 clear() powodowa³o b³±d
		}
		/**
			Nadpisane dla wyja¶nienia wieloznaczno¶ci		**/

		void setFont(const QFont& f) 	{ QTextBrowser::setFont(f); }
		void setMargin(int width);
		/**
			Returns path to image at position point, or null if there's no image.
		**/
		QString imageAt(const QPoint &point);

		/**
			default: false
		**/
		void setTrueTransparency(bool);
		bool isTrueTransparencyEnabled() const;

	public slots:
		virtual void copy();

	signals:
		/**
			Dowolny przycisk myszy zosta³ zwolniony
			sender() - obiekt, który wygenerowa³ zdarzenie
		**/
		void mouseReleased(QMouseEvent *e);

		/**
			U¿yto rolki myszy
			sender() - obiekt, który wygenerowa³ zdarzenie
		**/
		void wheel(QWheelEvent *e);
};

#endif
