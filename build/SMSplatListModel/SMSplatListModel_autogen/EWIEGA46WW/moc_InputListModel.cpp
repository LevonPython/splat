/****************************************************************************
** Meta object code from reading C++ file 'InputListModel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../SMSplatListModel/InputListModel.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'InputListModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSInputListModelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSInputListModelENDCLASS = QtMocHelpers::stringData(
    "InputListModel",
    "countChanged",
    "",
    "clear",
    "append",
    "InputInfoPtr",
    "info",
    "insert",
    "index",
    "remove",
    "removeByName",
    "name",
    "get",
    "getByName",
    "indexOf",
    "contains",
    "update",
    "updateByName",
    "names",
    "setItems",
    "QList<InputInfoPtr>",
    "items",
    "getByNameQml",
    "QtSMSplatInputInfo*",
    "duplicate",
    "count",
    "Roles",
    "TransmitterNameRole",
    "TransmitterLatRole",
    "TransmitterLonRole",
    "TransmitterAltRole",
    "ItmCovTypeRole",
    "ReceiverHeightRole",
    "StartAngleRole",
    "EndAngleRole",
    "RadiusRole",
    "FrequencyRole",
    "FresnelZoneRole",
    "InputInfoRole"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSInputListModelENDCLASS_t {
    uint offsetsAndSizes[78];
    char stringdata0[15];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[7];
    char stringdata5[13];
    char stringdata6[5];
    char stringdata7[7];
    char stringdata8[6];
    char stringdata9[7];
    char stringdata10[13];
    char stringdata11[5];
    char stringdata12[4];
    char stringdata13[10];
    char stringdata14[8];
    char stringdata15[9];
    char stringdata16[7];
    char stringdata17[13];
    char stringdata18[6];
    char stringdata19[9];
    char stringdata20[20];
    char stringdata21[6];
    char stringdata22[13];
    char stringdata23[20];
    char stringdata24[10];
    char stringdata25[6];
    char stringdata26[6];
    char stringdata27[20];
    char stringdata28[19];
    char stringdata29[19];
    char stringdata30[19];
    char stringdata31[15];
    char stringdata32[19];
    char stringdata33[15];
    char stringdata34[13];
    char stringdata35[11];
    char stringdata36[14];
    char stringdata37[16];
    char stringdata38[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSInputListModelENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSInputListModelENDCLASS_t qt_meta_stringdata_CLASSInputListModelENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14),  // "InputListModel"
        QT_MOC_LITERAL(15, 12),  // "countChanged"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 5),  // "clear"
        QT_MOC_LITERAL(35, 6),  // "append"
        QT_MOC_LITERAL(42, 12),  // "InputInfoPtr"
        QT_MOC_LITERAL(55, 4),  // "info"
        QT_MOC_LITERAL(60, 6),  // "insert"
        QT_MOC_LITERAL(67, 5),  // "index"
        QT_MOC_LITERAL(73, 6),  // "remove"
        QT_MOC_LITERAL(80, 12),  // "removeByName"
        QT_MOC_LITERAL(93, 4),  // "name"
        QT_MOC_LITERAL(98, 3),  // "get"
        QT_MOC_LITERAL(102, 9),  // "getByName"
        QT_MOC_LITERAL(112, 7),  // "indexOf"
        QT_MOC_LITERAL(120, 8),  // "contains"
        QT_MOC_LITERAL(129, 6),  // "update"
        QT_MOC_LITERAL(136, 12),  // "updateByName"
        QT_MOC_LITERAL(149, 5),  // "names"
        QT_MOC_LITERAL(155, 8),  // "setItems"
        QT_MOC_LITERAL(164, 19),  // "QList<InputInfoPtr>"
        QT_MOC_LITERAL(184, 5),  // "items"
        QT_MOC_LITERAL(190, 12),  // "getByNameQml"
        QT_MOC_LITERAL(203, 19),  // "QtSMSplatInputInfo*"
        QT_MOC_LITERAL(223, 9),  // "duplicate"
        QT_MOC_LITERAL(233, 5),  // "count"
        QT_MOC_LITERAL(239, 5),  // "Roles"
        QT_MOC_LITERAL(245, 19),  // "TransmitterNameRole"
        QT_MOC_LITERAL(265, 18),  // "TransmitterLatRole"
        QT_MOC_LITERAL(284, 18),  // "TransmitterLonRole"
        QT_MOC_LITERAL(303, 18),  // "TransmitterAltRole"
        QT_MOC_LITERAL(322, 14),  // "ItmCovTypeRole"
        QT_MOC_LITERAL(337, 18),  // "ReceiverHeightRole"
        QT_MOC_LITERAL(356, 14),  // "StartAngleRole"
        QT_MOC_LITERAL(371, 12),  // "EndAngleRole"
        QT_MOC_LITERAL(384, 10),  // "RadiusRole"
        QT_MOC_LITERAL(395, 13),  // "FrequencyRole"
        QT_MOC_LITERAL(409, 15),  // "FresnelZoneRole"
        QT_MOC_LITERAL(425, 13)   // "InputInfoRole"
    },
    "InputListModel",
    "countChanged",
    "",
    "clear",
    "append",
    "InputInfoPtr",
    "info",
    "insert",
    "index",
    "remove",
    "removeByName",
    "name",
    "get",
    "getByName",
    "indexOf",
    "contains",
    "update",
    "updateByName",
    "names",
    "setItems",
    "QList<InputInfoPtr>",
    "items",
    "getByNameQml",
    "QtSMSplatInputInfo*",
    "duplicate",
    "count",
    "Roles",
    "TransmitterNameRole",
    "TransmitterLatRole",
    "TransmitterLonRole",
    "TransmitterAltRole",
    "ItmCovTypeRole",
    "ReceiverHeightRole",
    "StartAngleRole",
    "EndAngleRole",
    "RadiusRole",
    "FrequencyRole",
    "FresnelZoneRole",
    "InputInfoRole"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSInputListModelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       1,  165, // properties
       1,  170, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  116,    2, 0x06,    3 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,  117,    2, 0x02,    4 /* Public */,
       4,    1,  118,    2, 0x02,    5 /* Public */,
       7,    2,  121,    2, 0x02,    7 /* Public */,
       9,    1,  126,    2, 0x02,   10 /* Public */,
      10,    1,  129,    2, 0x02,   12 /* Public */,
      12,    1,  132,    2, 0x102,   14 /* Public | MethodIsConst  */,
      13,    1,  135,    2, 0x102,   16 /* Public | MethodIsConst  */,
      14,    1,  138,    2, 0x102,   18 /* Public | MethodIsConst  */,
      15,    1,  141,    2, 0x102,   20 /* Public | MethodIsConst  */,
      16,    2,  144,    2, 0x02,   22 /* Public */,
      17,    2,  149,    2, 0x02,   25 /* Public */,
      18,    0,  154,    2, 0x102,   28 /* Public | MethodIsConst  */,
      19,    1,  155,    2, 0x02,   29 /* Public */,
      21,    0,  158,    2, 0x102,   31 /* Public | MethodIsConst  */,
      22,    1,  159,    2, 0x102,   32 /* Public | MethodIsConst  */,
      24,    1,  162,    2, 0x02,   34 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 5,    8,    6,
    QMetaType::Bool, QMetaType::Int,    8,
    QMetaType::Bool, QMetaType::QString,   11,
    0x80000000 | 5, QMetaType::Int,    8,
    0x80000000 | 5, QMetaType::QString,   11,
    QMetaType::Int, QMetaType::QString,   11,
    QMetaType::Bool, QMetaType::QString,   11,
    QMetaType::Bool, QMetaType::Int, 0x80000000 | 5,    8,    6,
    QMetaType::Bool, QMetaType::QString, 0x80000000 | 5,   11,    6,
    QMetaType::QStringList,
    QMetaType::Void, 0x80000000 | 20,   21,
    0x80000000 | 20,
    0x80000000 | 23, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   11,

 // properties: name, type, flags
      25, QMetaType::Int, 0x00015001, uint(0), 0,

 // enums: name, alias, flags, count, data
      26,   26, 0x0,   12,  175,

 // enum data: key, value
      27, uint(InputListModel::TransmitterNameRole),
      28, uint(InputListModel::TransmitterLatRole),
      29, uint(InputListModel::TransmitterLonRole),
      30, uint(InputListModel::TransmitterAltRole),
      31, uint(InputListModel::ItmCovTypeRole),
      32, uint(InputListModel::ReceiverHeightRole),
      33, uint(InputListModel::StartAngleRole),
      34, uint(InputListModel::EndAngleRole),
      35, uint(InputListModel::RadiusRole),
      36, uint(InputListModel::FrequencyRole),
      37, uint(InputListModel::FresnelZoneRole),
      38, uint(InputListModel::InputInfoRole),

       0        // eod
};

Q_CONSTINIT const QMetaObject InputListModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_CLASSInputListModelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSInputListModelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSInputListModelENDCLASS_t,
        // property 'count'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // enum 'Roles'
        QtPrivate::TypeAndForceComplete<InputListModel::Roles, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<InputListModel, std::true_type>,
        // method 'countChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clear'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'append'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const InputInfoPtr &, std::false_type>,
        // method 'insert'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const InputInfoPtr &, std::false_type>,
        // method 'remove'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'removeByName'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'get'
        QtPrivate::TypeAndForceComplete<InputInfoPtr, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'getByName'
        QtPrivate::TypeAndForceComplete<InputInfoPtr, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'indexOf'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'contains'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'update'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const InputInfoPtr &, std::false_type>,
        // method 'updateByName'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const InputInfoPtr &, std::false_type>,
        // method 'names'
        QtPrivate::TypeAndForceComplete<QStringList, std::false_type>,
        // method 'setItems'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QList<InputInfoPtr> &, std::false_type>,
        // method 'items'
        QtPrivate::TypeAndForceComplete<QList<InputInfoPtr>, std::false_type>,
        // method 'getByNameQml'
        QtPrivate::TypeAndForceComplete<QtSMSplatInputInfo *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'duplicate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void InputListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<InputListModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->countChanged(); break;
        case 1: _t->clear(); break;
        case 2: _t->append((*reinterpret_cast< std::add_pointer_t<InputInfoPtr>>(_a[1]))); break;
        case 3: _t->insert((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<InputInfoPtr>>(_a[2]))); break;
        case 4: { bool _r = _t->remove((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: { bool _r = _t->removeByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { InputInfoPtr _r = _t->get((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< InputInfoPtr*>(_a[0]) = std::move(_r); }  break;
        case 7: { InputInfoPtr _r = _t->getByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< InputInfoPtr*>(_a[0]) = std::move(_r); }  break;
        case 8: { int _r = _t->indexOf((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 9: { bool _r = _t->contains((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 10: { bool _r = _t->update((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<InputInfoPtr>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->updateByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<InputInfoPtr>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 12: { QStringList _r = _t->names();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 13: _t->setItems((*reinterpret_cast< std::add_pointer_t<QList<InputInfoPtr>>>(_a[1]))); break;
        case 14: { QList<InputInfoPtr> _r = _t->items();
            if (_a[0]) *reinterpret_cast< QList<InputInfoPtr>*>(_a[0]) = std::move(_r); }  break;
        case 15: { QtSMSplatInputInfo* _r = _t->getByNameQml((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QtSMSplatInputInfo**>(_a[0]) = std::move(_r); }  break;
        case 16: _t->duplicate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (InputListModel::*)();
            if (_t _q_method = &InputListModel::countChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<InputListModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->count(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *InputListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *InputListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSInputListModelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int InputListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 17;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void InputListModel::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
