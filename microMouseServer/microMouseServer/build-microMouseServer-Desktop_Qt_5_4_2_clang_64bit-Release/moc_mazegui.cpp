/****************************************************************************
** Meta object code from reading C++ file 'mazegui.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../microMouseServer/mazegui.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mazegui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_mazeGui_t {
    QByteArrayData data[11];
    char stringdata[122];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mazeGui_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mazeGui_t qt_meta_stringdata_mazeGui = {
    {
QT_MOC_LITERAL(0, 0, 7), // "mazeGui"
QT_MOC_LITERAL(1, 8, 13), // "passRightWall"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 4), // "cell"
QT_MOC_LITERAL(4, 28, 12), // "passLeftWall"
QT_MOC_LITERAL(5, 41, 11), // "passTopWall"
QT_MOC_LITERAL(6, 53, 14), // "passBottomWall"
QT_MOC_LITERAL(7, 68, 13), // "takeRightWall"
QT_MOC_LITERAL(8, 82, 12), // "takeLeftWall"
QT_MOC_LITERAL(9, 95, 11), // "takeTopWall"
QT_MOC_LITERAL(10, 107, 14) // "takeBottomWall"

    },
    "mazeGui\0passRightWall\0\0cell\0passLeftWall\0"
    "passTopWall\0passBottomWall\0takeRightWall\0"
    "takeLeftWall\0takeTopWall\0takeBottomWall"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mazeGui[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,
       5,    1,   60,    2, 0x06 /* Public */,
       6,    1,   63,    2, 0x06 /* Public */,
       7,    1,   66,    2, 0x06 /* Public */,
       8,    1,   69,    2, 0x06 /* Public */,
       9,    1,   72,    2, 0x06 /* Public */,
      10,    1,   75,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void, QMetaType::QPoint,    3,

       0        // eod
};

void mazeGui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        mazeGui *_t = static_cast<mazeGui *>(_o);
        switch (_id) {
        case 0: _t->passRightWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 1: _t->passLeftWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 2: _t->passTopWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 3: _t->passBottomWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 4: _t->takeRightWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 5: _t->takeLeftWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 6: _t->takeTopWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 7: _t->takeBottomWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (mazeGui::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mazeGui::passRightWall)) {
                *result = 0;
            }
        }
        {
            typedef void (mazeGui::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mazeGui::passLeftWall)) {
                *result = 1;
            }
        }
        {
            typedef void (mazeGui::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mazeGui::passTopWall)) {
                *result = 2;
            }
        }
        {
            typedef void (mazeGui::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mazeGui::passBottomWall)) {
                *result = 3;
            }
        }
        {
            typedef void (mazeGui::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mazeGui::takeRightWall)) {
                *result = 4;
            }
        }
        {
            typedef void (mazeGui::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mazeGui::takeLeftWall)) {
                *result = 5;
            }
        }
        {
            typedef void (mazeGui::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mazeGui::takeTopWall)) {
                *result = 6;
            }
        }
        {
            typedef void (mazeGui::*_t)(QPoint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mazeGui::takeBottomWall)) {
                *result = 7;
            }
        }
    }
}

const QMetaObject mazeGui::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_mazeGui.data,
      qt_meta_data_mazeGui,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *mazeGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mazeGui::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_mazeGui.stringdata))
        return static_cast<void*>(const_cast< mazeGui*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int mazeGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void mazeGui::passRightWall(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void mazeGui::passLeftWall(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void mazeGui::passTopWall(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void mazeGui::passBottomWall(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void mazeGui::takeRightWall(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void mazeGui::takeLeftWall(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void mazeGui::takeTopWall(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void mazeGui::takeBottomWall(QPoint _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
