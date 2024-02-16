#ifndef __LIBDATABRIDGE_FLAG__
#define __LIBDATABRIDGE_FLAG__

#include <string.h>
#include <iostream>
#include <vector>
#include <string>

#include "./libDataBridge.h"

//*   INFO CLASS   *//
namespace KPA
{
    class LV_OBJECT;
    class LVO_STRING;
    class LVO_INT32;
    class LVO_DOUBLE;
    class LVO_BOOL;
    std::vector<LV_OBJECT *> _lvobjs;
}
class KPA::LV_OBJECT
{
public:
    using KPA_REF = uint32_t;
    using GET_REF = KPA_REF (*)(KPA_REF bridge_num, char *label);
    static GET_REF getRef;

protected:
    const char *_label;
    KPA_REF ref_num;
    LV_OBJECT &__update_nominal()
    {
        return *this;
    }
    LV_OBJECT &(LV_OBJECT::*__p_update)() = __update_nominal;

public:
    LV_OBJECT(const char *label = "invalid", bool auto_update = false) : _label(label)
    {
        _lvobjs.push_back(this);
        if (auto_update)
            __p_update = update;
    }
    LV_OBJECT &link(KPA_REF bridge_num)
    {
        ref_num = (*getRef)(bridge_num, (char *)_label);
        return *this;
    }
    virtual LV_OBJECT &update() = 0;
    LV_OBJECT &operator=(const LV_OBJECT &source) = delete;
    LV_OBJECT &operator=(LV_OBJECT &&source) = delete;
};
KPA::LV_OBJECT::GET_REF KPA::LV_OBJECT::getRef = LibDataBridge_ReturnRef;

class KPA::LVO_STRING : public KPA::LV_OBJECT
{
public:
    using GET_STR = void (*)(KPA_REF Reference, LStrHandle *label);
    static GET_STR getStr;

protected:
    std::string _buf;
    LStrHandle _hdl;

public:
    LVO_STRING(const char *label = "invalid", bool auto_update = false) : LV_OBJECT(label, auto_update)
    {
        _hdl = (LStrHandle)DSNewHClr(8);
    }
    ~LVO_STRING()
    {
        DSDisposeHandle(_hdl);
    }
    LVO_STRING &update()
    {
        (*getStr)(ref_num, &_hdl);
        _buf.assign((char *)(*_hdl)->str, (*_hdl)->cnt);
        return *this;
    }
    char *c_str()
    {
        (this->*__p_update)();
        return (char *)_buf.c_str();
    }
    operator char *()
    {
        (this->*__p_update)();
        return (char *)_buf.c_str();
    }
};
KPA::LVO_STRING::GET_STR KPA::LVO_STRING::getStr = LibDataBridge_RefToStr;

class KPA::LVO_INT32 : public KPA::LV_OBJECT
{
public:
    using GET_INT = int32_t (*)(KPA_REF Reference);
    static GET_INT getInt;

private:
    int32_t _value;

public:
    LVO_INT32(const char *label, bool auto_update = false) : LV_OBJECT(label, auto_update)
    {
        _label = label;
    }
    LVO_INT32 &update()
    {
        _value = (*getInt)(ref_num);
        return *this;
    }
    operator int()
    {
        (this->*__p_update)();
        return _value;
    }
};
KPA::LVO_INT32::GET_INT KPA::LVO_INT32::getInt = LibDataBridge_RefToInt;

class KPA::LVO_DOUBLE : public KPA::LV_OBJECT
{
public:
    using GET_DBL = double (*)(KPA_REF Reference);
    static GET_DBL getDbl;

private:
    double _value;

public:
    LVO_DOUBLE(const char *label, bool auto_update = false) : LV_OBJECT(label, auto_update)
    {
        _label = label;
    }
    LVO_DOUBLE &update()
    {
        _value = (*getDbl)(ref_num);
        return *this;
    }
    operator double()
    {
        (this->*__p_update)();
        return _value;
    }
};
KPA::LVO_DOUBLE::GET_DBL KPA::LVO_DOUBLE::getDbl = LibDataBridge_RefToDbl;

class KPA::LVO_BOOL : public KPA::LV_OBJECT
{
public:
    using GET_BOOL = uint8_t (*)(KPA_REF Reference);
    static GET_BOOL getBool;

private:
    bool _value;

public:
    LVO_BOOL(const char *label, bool auto_update = false) : LV_OBJECT(label, auto_update)
    {
        _label = label;
    }
    LVO_BOOL &update()
    {
        _value = (*getBool)(ref_num);
        return *this;
    }
    operator bool()
    {
        (this->*__p_update)();
        return _value;
    }
};
KPA::LVO_BOOL::GET_BOOL KPA::LVO_BOOL::getBool = LibDataBridge_RefToBool;

//*   PORT CLASS   *//
namespace KPA
{
    class LV_PORT;
    class LVP_STRING;
    class LVP_INT32;
    class LVP_DOUBLE;
    class LVP_BOOL;
    std::vector<LV_PORT *> _lvports;
}
class KPA::LV_PORT
{
public:
    using KPA_REF = uint32_t;
    using GET_REF = KPA_REF (*)(KPA_REF bridge_num, char *label);
    static GET_REF getRef;

protected:
    const char *_label;
    KPA_REF ref_num;

public:
    LV_PORT(const char *label = "invalid") : _label(label)
    {
        _lvports.push_back(this);
    }
    LV_PORT &link(KPA_REF bridge_num)
    {
        ref_num = (*getRef)(bridge_num, (char *)_label);
        return *this;
    }
    LV_PORT &operator=(const LV_PORT &source) = delete;
    LV_PORT &operator=(LV_PORT &&source) = delete;
};
KPA::LV_PORT::GET_REF KPA::LV_PORT::getRef = LibDataBridge_ReturnRef;

class KPA::LVP_STRING : public KPA::LV_PORT
{
public:
    using GET_STR = void (*)(KPA_REF Reference, LStrHandle *Value);
    using SET_STR = void (*)(KPA_REF Reference, LStrHandle *Value);
    static GET_STR getStr;
    static SET_STR setStr;

public:
    LVP_STRING(const char *label = "invalid") : LV_PORT(label)
    {
        _label = label;
    }
    void operator<<(const char *str)
    {
        const size_t buf = 16;
        size_t len = strlen(str);
        LStrHandle hdl = (LStrHandle)DSNewHClr(len + buf);

        (*hdl)->cnt = len;
        strcpy((char *)(*hdl)->str, str);
        (*setStr)(ref_num, &hdl);

        DSDisposeHandle(hdl);
        return;
    }
    void operator<<(const std::string &str)
    {
        const size_t buf = 16;
        size_t len = str.size();
        LStrHandle hdl = (LStrHandle)DSNewHClr(len + buf);

        (*hdl)->cnt = len;
        memcpy((char *)(*hdl)->str, str.c_str(), len);
        (*setStr)(ref_num, &hdl);

        DSDisposeHandle(hdl);
        return;
    }
    void operator>>(std::string &str)
    {
        const size_t buf = 16;
        LStrHandle hdl = (LStrHandle)DSNewHClr(buf);

        (*getStr)(ref_num, &hdl);
        str.resize((*hdl)->cnt);
        memcpy((char *)str.c_str(), (*hdl)->str, (*hdl)->cnt);

        DSDisposeHandle(hdl);
        return;
    }
};
KPA::LVP_STRING::GET_STR KPA::LVP_STRING::getStr = LibDataBridge_RefToStr;
KPA::LVP_STRING::SET_STR KPA::LVP_STRING::setStr = LibDataBridge_RefFromStr;

class KPA::LVP_INT32 : public KPA::LV_PORT
{
public:
    using GET_INT = int32_t (*)(KPA_REF Reference);
    using SET_INT = void (*)(KPA_REF Reference, int32_t Value);
    static GET_INT getInt;
    static SET_INT setInt;

public:
    LVP_INT32(const char *label = "invalid") : LV_PORT(label)
    {
        _label = label;
    }
    void operator<<(int value)
    {
        (*setInt)(ref_num, value);
        return;
    }
    void operator>>(int *pointer)
    {
        *pointer = LibDataBridge_RefToInt(ref_num);
        return;
    }
};
KPA::LVP_INT32::GET_INT KPA::LVP_INT32::getInt = LibDataBridge_RefToInt;
KPA::LVP_INT32::SET_INT KPA::LVP_INT32::setInt = LibDataBridge_RefFromInt;

class KPA::LVP_DOUBLE : public KPA::LV_PORT
{
public:
    using GET_DBL = double (*)(KPA_REF Reference);
    using SET_DBL = void (*)(KPA_REF Reference, double Value);
    static GET_DBL getDbl;
    static SET_DBL setDbl;

public:
    LVP_DOUBLE(const char *label = "invalid") : LV_PORT(label)
    {
        _label = label;
    }
    void operator<<(double value)
    {
        (*setDbl)(ref_num, value);
        return;
    }
    void operator>>(double *pointer)
    {
        *pointer = (*getDbl)(ref_num);
        return;
    }
};
KPA::LVP_DOUBLE::GET_DBL KPA::LVP_DOUBLE::getDbl = LibDataBridge_RefToDbl;
KPA::LVP_DOUBLE::SET_DBL KPA::LVP_DOUBLE::setDbl = LibDataBridge_RefFromDbl;

class KPA::LVP_BOOL : public KPA::LV_PORT
{
public:
    using GET_BOOL = uint8_t (*)(KPA_REF Reference);
    using SET_BOOL = void (*)(KPA_REF Reference, uint8_t Value);
    static GET_BOOL getBool;
    static SET_BOOL setBool;

public:
    LVP_BOOL(const char *label = "invalid") : LV_PORT(label)
    {
        _label = label;
    }
    void operator<<(bool value)
    {
        (*setBool)(ref_num, value);
        return;
    }
    void operator>>(bool *pointer)
    {
        *pointer = (*getBool)(ref_num);
        return;
    }
};
KPA::LVP_BOOL::GET_BOOL KPA::LVP_BOOL::getBool = LibDataBridge_RefToBool;
KPA::LVP_BOOL::SET_BOOL KPA::LVP_BOOL::setBool = LibDataBridge_RefFromBool;

//*   Group control function   *//
namespace KPA
{
    void bridge_linkInfo(LV_OBJECT::KPA_REF bridge_num);
    void bridge_updateInfo();
}
void KPA::bridge_linkInfo(KPA::LV_OBJECT::KPA_REF bridge_num)
{
    for (int i = 0, len = _lvobjs.size(); i < len; i++)
        _lvobjs[i]->link(bridge_num);
    for (int i = 0, len = _lvports.size(); i < len; i++)
        _lvports[i]->link(bridge_num);
}
void KPA::bridge_updateInfo()
{
    for (int i = 0, len = _lvobjs.size(); i < len; i++)
        _lvobjs[i]->update();
}

//*   KPA C++ TEST ENVIRONMENT NECESSARY   *//
namespace KPA
{
    LVO_INT32 kpa_qty_sites{"Number of sites"};
    LVO_INT32 kpa_max_pass_bin{"Good bin limit"};
}

#endif // __LIBDATABRIDGE_FLAG__
