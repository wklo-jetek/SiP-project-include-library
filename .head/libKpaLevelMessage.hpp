#ifndef __LIBKPALEVELMESSAGE_HPP__
#define __LIBKPALEVELMESSAGE_HPP__

// #include <string.h>
#include <string>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <regex>
#include <queue>

#define str_format(stream, ...) ({                                   \
    size_t length = std::snprintf(NULL, 0, stream, __VA_ARGS__) + 1; \
    std::string str;                                                 \
    str.resize(length);                                              \
    std::snprintf((char *)str.c_str(), length, stream, __VA_ARGS__); \
    str.resize(length - 1);                                          \
    str;                                                             \
})

namespace KPA
{
    class MSG_INDENT;
    class LEVEL_MSG;
    double si_unit(char symbol);
} // namespace KPA

double KPA::si_unit(char symbol)
{
    switch (symbol)
    {
    case 'm':
        return 1E+3;
    case 'u':
        return 1E+6;
    case 'n':
        return 1E+9;
    case 'p':
        return 1E+12;
    case 'f':
        return 1E+15;
    case 'K':
        return 1E-3;
    case 'M':
        return 1E-6;
    case 'G':
        return 1E-9;
    case 'T':
        return 1E-12;
    default:
        return 1.0;
    }
    return 1.0;
}

class KPA::MSG_INDENT
{
private:
    static const char *space;
    static const char *space_end;
    static const uint8_t indent = 4;

public:
    static char *level;
    static void (*reset)(void);
    MSG_INDENT() { level = ((level - indent) >= space) ? level - indent : level; }
    ~MSG_INDENT() { level = ((level + indent) <= space_end) ? level + indent : level; }
};
const char *KPA::MSG_INDENT::space = "                                ";
const char *KPA::MSG_INDENT::space_end = space + strlen(space);
char *KPA::MSG_INDENT::level = (char *)KPA::MSG_INDENT::space_end;
void (*KPA::MSG_INDENT::reset)(void) = []() {
    level = (char *)space_end;
};

#define LEVEL_MSG_VER 2

#if LEVEL_MSG_VER == 2

#define istype(tp) std::is_same<T, tp>::value

class KPA::LEVEL_MSG
{
private:
    const char *p_form = NULL;
    const char *p_unit = NULL;
    std::string __prefix;
    std::string __postfix;

private:
    void __prtUnit();
    template <typename T>
    const char *__getFormat();
    void __prtFix(std::string &fix);

private: //* char*, const char*
    template <typename T>
    T anyValue(T value);

public: //* bool type
    bool operator<<(bool value);

public: //* int type
    int8_t operator<<(int8_t value);
    uint8_t operator<<(uint8_t value);
    int16_t operator<<(int16_t value);
    uint16_t operator<<(uint16_t value);
    int32_t operator<<(int32_t value);
    uint32_t operator<<(uint32_t value);
    int64_t operator<<(int64_t value);
    uint64_t operator<<(uint64_t value);

public: //* float type
    float operator<<(float value);
    double operator<<(double value);

public: //* string type
    char *operator<<(char *str);
    const char *operator<<(const char *str);
    std::string operator<<(std::string &&str);
    std::string &operator<<(std::string &str);
    const std::string &operator<<(const std::string &str);

public: //* vector type
    template <typename T>
    std::vector<T> operator<<(std::vector<T> &&data);
    template <typename T>
    std::vector<T> &operator<<(std::vector<T> &data);
    template <typename T>
    const std::vector<T> &operator<<(const std::vector<T> &data);

public: //* set mode
    LEVEL_MSG &unit(const char *_unit);
    LEVEL_MSG &format(const char *_format);
    template <typename T>
    LEVEL_MSG &prefix(const T &str);
    template <typename T>
    LEVEL_MSG &postfix(const T &str);
};
template <typename T>
T KPA::LEVEL_MSG::anyValue(T value)
{
    std::cout << MSG_INDENT::level;
    this->__prtFix(this->__prefix);

    if constexpr (istype(char *) || istype(const char *))
        std::cout << value;
    if constexpr (istype(int8_t) || istype(int16_t) || istype(int32_t) || istype(int64_t) ||
                  istype(uint8_t) || istype(uint16_t) || istype(uint32_t) || istype(uint64_t))
        printf(__getFormat<T>(), value);
    if constexpr (istype(float) || istype(double))
    {
        printf(__getFormat<T>(), value * ((p_unit != NULL) ? si_unit(*p_unit) : 1.0));
        this->__prtUnit();
    }
    this->__prtFix(this->__postfix);
    std::cout << std::endl;
    p_form = NULL;
    return value;
}
bool KPA::LEVEL_MSG::operator<<(bool value)
{
    std::cout << MSG_INDENT::level;
    this->__prtFix(this->__prefix);
    std::cout << std::boolalpha << value;
    this->__prtFix(this->__postfix);
    std::cout << std::endl;
    return value;
}
std::string KPA::LEVEL_MSG::operator<<(std::string &&str)
{
    std::cout << MSG_INDENT::level;
    this->__prtFix(this->__prefix);
    std::cout << str;
    this->__prtFix(this->__postfix);
    std::cout << std::endl;
    return str;
}
std::string &KPA::LEVEL_MSG::operator<<(std::string &str)
{
    std::cout << MSG_INDENT::level;
    this->__prtFix(this->__prefix);
    std::cout << str;
    this->__prtFix(this->__postfix);
    std::cout << std::endl;
    return str;
}
const std::string &KPA::LEVEL_MSG::operator<<(const std::string &str)
{
    std::cout << MSG_INDENT::level;
    this->__prtFix(this->__prefix);
    std::cout << str;
    this->__prtFix(this->__postfix);
    std::cout << std::endl;
    return str;
}

template <typename T>
std::vector<T> KPA::LEVEL_MSG::operator<<(std::vector<T> &&data)
{
    std::cout << MSG_INDENT::level;
    this->__prtFix(this->__prefix);
    const char *_format = __getFormat<T>();
    double _mul;

    if constexpr (istype(float) || istype(double))
    {
        _mul = (p_unit != NULL) ? si_unit(*p_unit) : 1.0;
        __prtUnit();
    }

    auto print = [&_format, &_mul](T ele) {
        if constexpr (istype(float) || istype(double))
            printf(_format, ele * _mul);
        else if constexpr (istype(std::string))
            printf(_format, ele.c_str());
        else if constexpr (istype(bool))
            std::cout << ele;
        else
            printf(_format, ele);
    };

    std::cout << "[";
    if (data.size() != 0)
    {
        print(data[0]);
        for (size_t i = 1; i < data.size(); i++)
        {
            std::cout << ", ";
            print(data[i]);
        }
    }
    std::cout << "]";
    this->__prtFix(this->__postfix);
    std::cout << std::endl;

    return data;
}
template <typename T>
std::vector<T> &KPA::LEVEL_MSG::operator<<(std::vector<T> &data)
{
    this->operator<<(std::move(data));
    return data;
}
template <typename T>
const std::vector<T> &KPA::LEVEL_MSG::operator<<(const std::vector<T> &data)
{
    this->operator<<(std::move((std::vector<T>)data));
    return data;
}

#define printValueType(tp) \
    tp KPA::LEVEL_MSG::operator<<(tp value) { return anyValue(value); }
printValueType(char *);
printValueType(const char *);
printValueType(int8_t);
printValueType(uint8_t);
printValueType(int16_t);
printValueType(uint16_t);
printValueType(int32_t);
printValueType(uint32_t);
printValueType(int64_t);
printValueType(uint64_t);
printValueType(float);
printValueType(double);
#undef printValueType

KPA::LEVEL_MSG &KPA::LEVEL_MSG::unit(const char *_unit)
{
    p_unit = _unit;
    return *this;
}
KPA::LEVEL_MSG &KPA::LEVEL_MSG::format(const char *_format)
{
    p_form = _format;
    return *this;
}
template <typename T>
KPA::LEVEL_MSG &KPA::LEVEL_MSG::prefix(const T &str)
{
    this->__prefix = str;
    return *this;
}
template <typename T>
KPA::LEVEL_MSG &KPA::LEVEL_MSG::postfix(const T &str)
{
    this->__postfix = str;
    return *this;
}

void KPA::LEVEL_MSG::__prtUnit()
{
    if (p_unit == NULL)
        return;
    std::cout << "(" << p_unit << ")";
    p_unit = NULL;
}
template <typename T>
const char *KPA::LEVEL_MSG::__getFormat()
{
    const char *out = "";
    if constexpr (istype(char *) || istype(const char *) || istype(std::string))
        out = (p_form != NULL) ? p_form : "\"%s\"";
    if constexpr (istype(int8_t) || istype(uint8_t))
        out = (p_form != NULL) ? p_form : "0x%02X";
    if constexpr (istype(int16_t) || istype(uint16_t) ||
                  istype(int32_t) || istype(uint32_t))
        out = (p_form != NULL) ? p_form : "%d";
    if constexpr (istype(int64_t) || istype(uint64_t))
        out = (p_form != NULL) ? p_form : "%lld";
    if constexpr (istype(float) || istype(double))
        out = (p_form != NULL) ? p_form : "%g";
    if constexpr (istype(bool))
        std::cout << std::boolalpha;
    p_form = NULL;
    return out;
}
void KPA::LEVEL_MSG::__prtFix(std::string &fix)
{
    if (fix.length() == 0)
        return;
    std::cout << fix;
    fix.clear();
}

#undef istype

#endif //* LEVEL_MSG_VER == 2

#if LEVEL_MSG_VER == 1
class KPA::LEVEL_MSG
{
public:
    using METHOD = std::ios_base &(*)(std::ios_base &);

private:
    std::queue<METHOD> methods;
    const char *p_unit = NULL;
    const char *p_form = NULL;
    std::string b_prefix = "", b_postfix = "";

    void __setin();
    template <typename T>
    void __settype();
    void __setdefault();
    template <typename T>
    const char *__setType();
    template <typename T>
    void __anyPrint(T value, const char *_form);
    double __unit(char symbol);
    void __prtFix(std::string &b_fix);

public:
    LEVEL_MSG(){};
    KPA::LEVEL_MSG &operator<<(METHOD method);
    template <class O>
    KPA::LEVEL_MSG &operator<<(METHOD method);
    KPA::LEVEL_MSG &unit(const char *_unit);
    KPA::LEVEL_MSG &format(const char *_format);
    KPA::LEVEL_MSG &prefix(std::string &&str);
    KPA::LEVEL_MSG &postfix(std::string &&str);
    template <typename T>
    T operator<<(T value);
    char *operator<<(char *);
    const char *operator<<(const char *);
    std::string operator<<(std::string &&value);
    const std::string &operator<<(const std::string &value);

    template <typename T>
    std::vector<T> operator<<(std::vector<T> &&data);
    template <typename T>
    std::vector<T> &operator<<(std::vector<T> &data);
    template <typename T>
    const std::vector<T> &operator<<(const std::vector<T> &data);
};
void KPA::LEVEL_MSG::__setin()
{
    std::cout << MSG_INDENT::level;
    while (!methods.empty())
    {
        std::cout << methods.front();
        methods.pop();
    }
}
template <typename T>
void KPA::LEVEL_MSG::__settype()
{
    // std::cout << std::setw(0);
    if constexpr (std::is_same<T, bool>::value)
        std::cout << std::showbase << std::dec;
    else if constexpr (std::is_same<T, int8_t>::value ||
                       std::is_same<T, uint8_t>::value)
        std::cout << std::showbase << std::hex;
    else if constexpr (std::is_same<T, int16_t>::value ||
                       std::is_same<T, uint16_t>::value ||
                       std::is_same<T, int32_t>::value ||
                       std::is_same<T, uint32_t>::value ||
                       std::is_same<T, int64_t>::value ||
                       std::is_same<T, uint64_t>::value)
        std::cout << std::showbase << std::dec;
    // else if constexpr (std::is_same<T, double>::value ||
    //                    std::is_same<T, float>::value)
    //     std::cout << std::showbase << std::dec;
}
void KPA::LEVEL_MSG::__setdefault()
{
    std::cout << std::setw(0);
}
template <typename T>
const char *KPA::LEVEL_MSG::__setType()
{
    if constexpr (std::is_same<T, int8_t>::value ||
                  std::is_same<T, uint8_t>::value)
        return (p_form != NULL) ? p_form : "0x%02X";
    else if constexpr (std::is_same<T, int16_t>::value ||
                       std::is_same<T, uint16_t>::value ||
                       std::is_same<T, int32_t>::value ||
                       std::is_same<T, uint32_t>::value ||
                       std::is_same<T, int64_t>::value ||
                       std::is_same<T, uint64_t>::value)
        return (p_form != NULL) ? p_form : "%d";
    else
        return "%s";
}
template <typename T>
void KPA::LEVEL_MSG::__anyPrint(T value, const char *_form)
{
}
double KPA::LEVEL_MSG::__unit(char symbol)
{
    switch (symbol)
    {
    case 'm':
        return 1E+3;
    case 'u':
        return 1E+6;
    case 'n':
        return 1E+9;
    case 'p':
        return 1E+12;
    case 'f':
        return 1E+15;
    case 'K':
        return 1E-3;
    case 'M':
        return 1E-6;
    case 'G':
        return 1E-9;
    case 'T':
        return 1E-12;
    default:
        return 1.0;
    }
    return 1.0;
}
void KPA::LEVEL_MSG::__prtFix(std::string &b_fix)
{
    if (b_fix.length() != 0)
    {
        std::cout << b_fix;
        b_fix.clear();
    }
}
KPA::LEVEL_MSG &KPA::LEVEL_MSG::operator<<(KPA::LEVEL_MSG::METHOD method)
{
    methods.push(method);
    return *this;
}

KPA::LEVEL_MSG &KPA::LEVEL_MSG::unit(const char *_unit)
{
    p_unit = _unit;
    return *this;
}
KPA::LEVEL_MSG &KPA::LEVEL_MSG::format(const char *_format)
{
    p_form = _format;
    return *this;
}
KPA::LEVEL_MSG &KPA::LEVEL_MSG::prefix(std::string &&str)
{
    b_prefix = str;
    return *this;
}
KPA::LEVEL_MSG &KPA::LEVEL_MSG::postfix(std::string &&str)
{
    b_postfix = str;
    return *this;
}

template <typename T>
T KPA::LEVEL_MSG::operator<<(T value)
{
    this->__settype<T>();
    this->__setin();
    this->__prtFix(this->b_prefix);
    // const char *__form = this->__setType<T>();

    // if constexpr (std::is_same<T, int8_t>::value)
    //     std::cout << int32_t(value) << std::endl;
    // else if constexpr (std::is_same<T, uint8_t>::value)
    //     std::cout << uint32_t(value) << std::endl;
    if constexpr (std::is_same<T, int8_t>::value ||
                  std::is_same<T, uint8_t>::value ||
                  std::is_same<T, int16_t>::value ||
                  std::is_same<T, uint16_t>::value ||
                  std::is_same<T, int32_t>::value ||
                  std::is_same<T, uint32_t>::value ||
                  std::is_same<T, int64_t>::value ||
                  std::is_same<T, uint64_t>::value)
        printf(this->__setType<T>(), value);
    else if constexpr (std::is_same<T, bool>::value)
        std::cout << else if constexpr (std::is_same<T, double>::value ||
                                        std::is_same<T, float>::value)
        {
            if (p_unit == NULL)
            {
                std::cout << value << std::endl;
            }
            else
            {
                std::cout << (value * __unit(*p_unit)) << "(" << p_unit << ")" << std::endl;
                p_unit = NULL;
            }
        }
    else
        std::cout << value << std::endl;

    this->__prtFix(this->b_postfix);
    return value;
}
char *KPA::LEVEL_MSG::operator<<(char *value)
{
    this->__setin();
    std::cout << value << std::endl;
    return value;
}
const char *KPA::LEVEL_MSG::operator<<(const char *value)
{
    this->__setin();
    std::cout << value << std::endl;
    return value;
}
std::string KPA::LEVEL_MSG::operator<<(std::string &&value)
{
    this->__setin();
    std::cout << value << std::endl;
    return value;
}
const std::string &KPA::LEVEL_MSG::operator<<(const std::string &value)
{
    this->operator<<(std::move(value));
    return value;
}

template <typename T>
std::vector<T> KPA::LEVEL_MSG::operator<<(std::vector<T> &&data)
{
    double v_mul;
    this->__settype<T>();
    this->__setin();
    this->__prtFix(this->b_prefix);
    const char *__form = this->__setType<T>();

    if constexpr (std::is_same<T, double>::value ||
                  std::is_same<T, float>::value)
    {
        if (p_unit == NULL)
        {
            v_mul = 1.0;
        }
        else
        {
            v_mul = (__unit(*p_unit));
            std::cout << "(" << p_unit << ")";
            p_unit = NULL;
        }
    }

    auto prt = [&v_mul, __form](T ele) {
        // if constexpr (std::is_same<T, int8_t>::value)
        //     std::cout << int32_t(ele);
        // else if constexpr (std::is_same<T, uint8_t>::value)
        //     std::cout << uint32_t(ele);
        if constexpr (std::is_same<T, int8_t>::value ||
                      std::is_same<T, uint8_t>::value ||
                      std::is_same<T, int16_t>::value ||
                      std::is_same<T, uint16_t>::value ||
                      std::is_same<T, int32_t>::value ||
                      std::is_same<T, uint32_t>::value ||
                      std::is_same<T, int64_t>::value ||
                      std::is_same<T, uint64_t>::value)
            printf(__form, ele);
        else if constexpr (std::is_same<T, double>::value ||
                           std::is_same<T, float>::value)
            std::cout << ele * v_mul;
        else if constexpr (std::is_same<T, std::string>::value ||
                           std::is_same<T, const char *>::value)
            std::cout << "\"" << ele << "\"";
        else
            std::cout << ele;
    };

    std::cout << "[";
    if (data.size() != 0)
    {
        prt(data[0]);
        for (size_t i = 1; i < data.size(); i++)
        {
            std::cout << ", ";
            prt(data[i]);
        }
    }
    std::cout << "]" << std::endl;

    this->__prtFix(this->b_postfix);
    return data;
}
template <typename T>
std::vector<T> &KPA::LEVEL_MSG::operator<<(std::vector<T> &data)
{
    this->operator<<(std::move(data));
    return data;
}
template <typename T>
const std::vector<T> &KPA::LEVEL_MSG::operator<<(const std::vector<T> &data)
{
    this->operator<<(std::move((std::vector<T>)data));
    return data;
}

#define MSG_EXT(O)                                 \
    LEVEL_MSG &operator<<(LEVEL_MSG &__msg, O ins) \
    {                                              \
        std::cout << ins;                          \
        return __msg;                              \
    }

namespace KPA
{
    MSG_EXT(std::_Setfill<char>)
    MSG_EXT(std::_Setiosflags)
    MSG_EXT(std::_Resetiosflags)
    MSG_EXT(std::_Setprecision)
    MSG_EXT(std::_Setw)
} // namespace KPA
#endif //* LEVEL_MSG_VER == 1

namespace KPA
{
    KPA::LEVEL_MSG msg;
}

#endif //* __LIBKPALEVELMESSAGE_HPP__