/*
 * This is the searchbox class code by Matteo Bertozzi:
 * http://th30z.netsons.org/2008/08/qt4-mac-searchbox-wrapper/
 */

// Qt4 Headers
#include <QtCore/QVarLengthArray>
#include <QtGui/QWidget>

// SearchBox Headers
#include "searchbox_mac.h"

// =======================================
//  MacSearchBox: PRIVATE FILE Methods
// =======================================
static QString toQString(CFStringRef str) {
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

static OSStatus SearchFieldEventHandler(EventHandlerCallRef handlerCallRef,
                                        EventRef event, void *userData)
{
    QMacSearchBox *searchBox = (QMacSearchBox *) userData;
    OSType eventClass = GetEventClass(event);
    UInt32 eventKind = GetEventKind(event);

    if (eventClass == kEventClassSearchField) {
        switch (eventKind) {
            case kEventSearchFieldCancelClicked:
                searchBox->clear();
                break;
            case kEventSearchFieldSearchClicked:
                searchBox->raiseFindNext();
                break;
            default:
                break;
        }
    } else if (eventClass == kEventClassTextField) {
        switch (eventKind) {
            case kEventTextDidChange:
                searchBox->raiseTextChanged();
                break;
            case kEventTextAccepted:
                searchBox->raiseFindNext();
                break;
            default:
                break;
        }
    }

    return(eventNotHandledErr);
}

// =======================================
//  MacSearchBox: PUBLIC Constructors/Destructors
// =======================================
QMacSearchBox::QMacSearchBox(QWidget *parent) : QWidget(parent)
{
    // Create a native search field and pass its window id to QWidget::create.
    searchFieldText = CFStringCreateWithCString(0,
                                     (const char *) tr("Search").toAscii(), 0);

    HISearchFieldCreate(NULL, kHISearchFieldAttributesSearchIcon |
                              kHISearchFieldAttributesCancel,
                        NULL, searchFieldText, &searchField);

    create(reinterpret_cast<WId>(searchField));

    // Subscribe Events
    EventTypeSpec mySFieldEvents[] = {
                     { kEventClassSearchField, kEventSearchFieldCancelClicked },
                     { kEventClassTextField, kEventTextDidChange },
                     { kEventClassTextField, kEventTextAccepted }
                 };

    HIViewInstallEventHandler(searchField, SearchFieldEventHandler,
                              GetEventTypeCount(mySFieldEvents), mySFieldEvents,
                              (void *) this, NULL);
}

QMacSearchBox::~QMacSearchBox() {
    CFRelease(searchField);
    CFRelease(searchFieldText);
}

// =======================================
//  MacSearchBox: PUBLIC Methods
// =======================================
void QMacSearchBox::raiseTextChanged (void) {
    emit textChanged(text());
}

void QMacSearchBox::raiseFindNext (void) {
    emit findNext();
}

QSize QMacSearchBox::sizeHint (void) const {
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
    return QSize(optimalBounds.size.width + 200, optimalBounds.size.height);
}

// =======================================
//  MacSearchBox: PUBLIC GET Properties
// =======================================
QString QMacSearchBox::text (void) const {
    CFStringRef cfString = HIViewCopyText(searchField);
    QString text = toQString(cfString);
    CFRelease(cfString);
    return(text);
}

// =======================================
//  MacSearchBox: PUBLIC SET Properties
// =======================================
void QMacSearchBox::setText (const QString& text) {
    CFRelease(searchFieldText);

    searchFieldText = CFStringCreateWithCString(0,
                          (const char *) text.toAscii(), 0);
    HIViewSetText(searchField, searchFieldText);

    emit textChanged(text);
}

void QMacSearchBox::clear (void) {
    setText(QString());
}

void QMacSearchBox::activate(void) {
	HIViewAdvanceFocus(searchField, 0);
	/* Dorr: I don't know the Carbon API. The HIViewAdvanceFocus was the only one
	 * method to pass the focus to searchbox. However I have to reset the text here
	 * otherwise some letters will be duplicated */
	setText(text());
}
