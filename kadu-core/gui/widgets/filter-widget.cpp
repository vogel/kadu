/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Mac OS X implementation based on searchbox class by Matteo Bertozzi:
 * http://th30z.netsons.org/2008/08/qt4-mac-searchbox-wrapper/
 */

#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "gui/widgets/line-edit-with-clear-button.h"
#include "gui/widgets/talkable-tree-view.h"

#include "filter-widget.h"

#ifdef Q_OS_MAC
#include <QtCore/QVarLengthArray>

static QString toQString(CFStringRef str)
{
	if(!str)
		return QString();

	CFIndex length = CFStringGetLength(str);
	const UniChar *chars = CFStringGetCharactersPtr(str);
	if (chars)
		return QString(reinterpret_cast<const QChar *>(chars), length);

	QVarLengthArray<UniChar> buffer(length);
	CFStringGetCharacters(str, CFRangeMake(0, length), buffer.data());
	return QString(reinterpret_cast<const QChar *>(buffer.constData()), length);
}

static OSStatus FilterFieldEventHandler(EventHandlerCallRef handlerCallRef,
					EventRef event, void *userData)
{
	Q_UNUSED(handlerCallRef);
	FilterWidget *filter = (FilterWidget *) userData;
	OSType eventClass = GetEventClass(event);
	UInt32 eventKind = GetEventKind(event);

	if (eventClass == kEventClassSearchField && eventKind == kEventSearchFieldCancelClicked)
		filter->clear();
	else if (eventClass == kEventClassTextField && eventKind == kEventTextDidChange)
		filter->emitTextChanged();
	return (eventNotHandledErr);
}

void FilterWidget::emitTextChanged(void)
{
	filterTextChanged(text());
}

void FilterWidget::clear(void)
{
	setText(QString());
}

QString FilterWidget::text(void) const
{
	CFStringRef cfString = HIViewCopyText(searchField);
	QString text = toQString(cfString);
	CFRelease(cfString);
	return(text);
}

void FilterWidget::setText(const QString &text)
{
	CFRelease(searchFieldText);
	searchFieldText = CFStringCreateWithCString(0,
			text.toUtf8().constData(), 0);
	HIViewSetText(searchField, searchFieldText);
	emit textChanged(text);
}

void FilterWidget::activate(void)
{
	SetKeyboardFocus(HIViewGetWindow(searchField), searchField, kControlFocusNoPart);
	SetKeyboardFocus(HIViewGetWindow(searchField), searchField, kControlFocusNextPart);
	setText(text());
}

QSize FilterWidget::sizeHint (void) const
{
	HIRect optimalBounds;
	EventRef event;
	CreateEvent(0, kEventClassControl, kEventControlGetOptimalBounds,
		GetCurrentEventTime(), kEventAttributeUserEvent, &event);
	SendEventToEventTargetWithOptions(event,
		HIObjectGetEventTarget(HIObjectRef(winId())),
		kEventTargetDontPropagate);
	GetEventParameter(event, kEventParamControlOptimalBounds, typeHIRect,
		0, sizeof(HIRect), 0, &optimalBounds);
	ReleaseEvent(event);
	return QSize(optimalBounds.size.width + 200, optimalBounds.size.height - 4);
}
#endif

void FilterWidget::filterTextChanged(const QString &s)
{
	emit textChanged(s);

	if (!View)
		return;

#ifdef Q_OS_MAC
	if (text().isEmpty())
#else
	if (NameFilterEdit->text().isEmpty())
#endif
	{
		QModelIndexList selection = View->selectionModel()->selectedIndexes();
		if (!selection.isEmpty())
		{
			qSort(selection);
			View->scrollTo(selection.at(0));
		}
		// clearFocus() is needed to ensure that focus on View wiil be restored
		// even if this function had been called at time KaduWindow was not active
		clearFocus();
		// from qdocs: "[It works] if this widget or one of its parents is the active window"
		View->setFocus(Qt::OtherFocusReason);
#ifndef Q_OS_MAC
		hide();
#endif
	}
	else
	{
		if (!isVisible() || View->selectionModel()->selectedIndexes().isEmpty())
		{
			View->setCurrentIndex(View->model()->index(0, 0));
			View->selectionModel()->select(View->model()->index(0, 0), QItemSelectionModel::SelectCurrent);
		}
#ifndef Q_OS_MAC
		show();
#endif
	}
}

FilterWidget::FilterWidget(QWidget *parent) : QWidget(parent)
{
	View = 0;

#ifdef Q_OS_MAC

	searchFieldText = CFStringCreateWithCString(0,
		tr("Search").toUtf8().constData(), 0);
	HISearchFieldCreate(NULL, kHISearchFieldAttributesSearchIcon |
		kHISearchFieldAttributesCancel,
		NULL, searchFieldText, &searchField);
	create(reinterpret_cast<WId>(searchField));
	EventTypeSpec mySFieldEvents[] = {
		{ kEventClassSearchField, kEventSearchFieldCancelClicked },
		{ kEventClassTextField, kEventTextDidChange },
		{ kEventClassTextField, kEventTextAccepted }
	};
	HIViewInstallEventHandler(searchField, FilterFieldEventHandler,
		GetEventTypeCount(mySFieldEvents), mySFieldEvents,
		(void *) this, NULL);

#elif !defined(Q_WS_MAEMO_5)


	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(3);

	NameFilterEdit = new LineEditWithClearButton(this);

	setFocusProxy(NameFilterEdit);

	layout->addWidget(new QLabel(tr("Search") + ':', this));
	layout->addWidget(NameFilterEdit);

	connect(NameFilterEdit, SIGNAL(textChanged(const QString &)),
			this, SLOT(filterTextChanged(const QString &)));
#endif
}

FilterWidget::~FilterWidget()
{
#ifdef Q_OS_MAC
	CFRelease(searchField);
	CFRelease(searchFieldText);
#endif
}

void FilterWidget::setFilter(const QString &filter)
{
#ifdef Q_OS_MAC
	if (text().isEmpty())
		setText(filter);
	activate();
#elif !defined(Q_WS_MAEMO_5)
	NameFilterEdit->setText(filter);
#else
	Q_UNUSED(filter);
#endif
}

void FilterWidget::setView(QTreeView *view)
{
	View = view;
}

bool FilterWidget::sendKeyEventToView(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Down:
		case Qt::Key_Up:
		case Qt::Key_PageDown:
		case Qt::Key_PageUp:
			qApp->sendEvent(View, event);
			return true;
	}

	return false;
}

void FilterWidget::keyPressEvent(QKeyEvent *event)
{
#ifndef Q_WS_MAEMO_5
	if (event->key() == Qt::Key_Escape &&
#ifdef Q_OS_MAC
			!text().isEmpty()
#else
			!NameFilterEdit->text().isEmpty()
#endif
			)
	{
		setFilter(QString());
		event->accept();
		return;
	}

	if (View && sendKeyEventToView(event))
		return;
#endif // !Q_WS_MAEMO_5

	QWidget::keyPressEvent(event);
}
