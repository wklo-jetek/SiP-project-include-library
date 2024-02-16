namespace KPA
{
#include ".\libKpaCmdSet.h"
}

namespace KPA
{
    struct ITEM_SELECT
    {
    public:
        using NEXT_C_FUNC = int (*)(char *, int);

    private:
        NEXT_C_FUNC next_c_func;

    public:
        std::string str;
        ITEM_SELECT(NEXT_C_FUNC func) : next_c_func(func){};
        bool next()
        {
            const int len = 128;
            char buf[len + 1];
            bool flag = next_c_func(buf, len);
            str = (flag) ? buf : (char *)"";
            return flag;
        }
        void reset()
        {
            str = "";
        }
        operator const std::string &()
        {
            return str;
        }
        const char *c_str()
        {
            return str.c_str();
        }
        template <typename T>
        bool is(T strin)
        {
            return (str == strin);
        }
        template <typename T>
        bool operator==(T strin)
        {
            return is(strin);
        }
    };
}

//* instructions
namespace KPA
{
    template <typename T>
    T *c_ptr(std::vector<T> &data);
}
template <typename T>
T *KPA::c_ptr(std::vector<T> &data)
{
    return (data.size() > 0) ? &data[0] : NULL;
}