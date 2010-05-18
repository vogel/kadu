/****************************************************************************
** Meta object code from reading C++ file 'growlnotifier.h'
**
** Created: Tue May 18 18:26:47 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "growlnotifier.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'growlnotifier.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GrowlNotifierSignaler[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,
      50,   22,   22,   22, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_GrowlNotifierSignaler[] = {
    "GrowlNotifierSignaler\0\0"
    "notificationClicked(void*)\0"
    "notificationTimedOut(void*)\0"
};

const QMetaObject GrowlNotifierSignaler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GrowlNotifierSignaler,
      qt_meta_data_GrowlNotifierSignaler, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GrowlNotifierSignaler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GrowlNotifierSignaler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GrowlNotifierSignaler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GrowlNotifierSignaler))
        return static_cast<void*>(const_cast< GrowlNotifierSignaler*>(this));
    return QObject::qt_metacast(_clname);
}

int GrowlNotifierSignaler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: notificationClicked((*reinterpret_cast< void*(*)>(_a[1]))); break;
        case 1: notificationTimedOut((*reinterpret_cast< void*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GrowlNotifierSignaler::notificationClicked(void * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GrowlNotifierSignaler::notificationTimedOut(void * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
