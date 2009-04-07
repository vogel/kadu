#ifdef Q_OS_MAC

#ifndef _QMAC_SEARCHBOX_H_
#define _QMAC_SEARCHBOX_H_

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <Carbon/Carbon.h>

class QMacSearchBox : public QWidget
{
    Q_OBJECT

    public:
        QMacSearchBox(QWidget *parent = 0);
        ~QMacSearchBox();

        // Methods
        void raiseTextChanged (void);
        void raiseFindNext (void);

        QSize sizeHint(void) const;

        // GET Properties
        QString text(void) const;

    signals:
        void textChanged (const QString& text);
        void findNext(void);

    public slots:
        void setText (const QString& text);
        void clear(void);

    private:
        CFStringRef searchFieldText;
        HIViewRef searchField;
};

#endif /* _QMAC_SEARCHBOX_H_ */

#endif /* Q_OS_MAC */
