
#ifndef FTD3XX_H
#ifndef FTD2XX_H

#include "ftd2xx.h"
#include "libCustomizeItem.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

enum FT_STATUS
{
    FT_OK,
    FT_INVALID_HANDLE,
    FT_DEVICE_NOT_FOUND,
    FT_DEVICE_NOT_OPENED,
    FT_IO_ERROR,
    FT_INSUFFICIENT_RESOURCES,
    FT_INVALID_PARAMETER
};

//
// FT_ListDevices Flags (used in conjunction with FT_OpenEx Flags
//

#define FT_LIST_NUMBER_ONLY 0x80000000
#define FT_LIST_BY_INDEX 0x40000000
#define FT_LIST_ALL 0x20000000

//
// Purge rx and tx buffers
//

#define FT_PURGE_RX 1
#define FT_PURGE_TX 2

#define FT_OPEN_BY_SERIAL_NUMBER 0x00000001
#define FT_OPEN_BY_DESCRIPTION 0x00000002
#define FT_OPEN_BY_LOCATION 0x00000004
#define FT_OPEN_BY_GUID 0x00000008
#endif //* FTD2XX_H
#define FT_OPEN_BY_INDEX 0x00000010
#endif //* FTD3XX_H

typedef unsigned long long FT_HANDLE;

class FTD_SERIES
{
protected:
    HMODULE lib;
    using ptrToListDevices = FT_STATUS(WINAPI *)(PVOID, PVOID, DWORD);
    ptrToListDevices __ListDevices;
    using ptrToClose = FT_STATUS(WINAPI *)(FT_HANDLE);
    ptrToClose __Close;
    using ptrToResetDevice = FT_STATUS(WINAPI *)(FT_HANDLE);
    ptrToResetDevice __ResetDevice;

public:
    FTD_SERIES(const char *lib_path);
    ~FTD_SERIES();
    std::vector<std::string> ListDevices();
    FT_STATUS Close(FT_HANDLE hdl);
    FT_STATUS ResetDevice(FT_HANDLE hdl);
};
class FTD2 : public FTD_SERIES
{
protected:
    using ptrToOpen = FT_STATUS(WINAPI *)(int, FT_HANDLE *);
    ptrToOpen __Open;
    using ptrToOpenEx = FT_STATUS(WINAPI *)(PVOID, DWORD, FT_HANDLE *);
    ptrToOpenEx __OpenEx;
    using ptrToWrite = FT_STATUS(WINAPI *)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
    ptrToWrite __Write;
    using ptrToRead = FT_STATUS(WINAPI *)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
    ptrToRead __Read;
    using ptrToGetQueueStatus = FT_STATUS(WINAPI *)(FT_HANDLE, DWORD *);
    ptrToGetQueueStatus __GetQueueStatus;
    using ptrToSetBaudRate = FT_STATUS(WINAPI *)(FT_HANDLE, ULONG);
    ptrToSetBaudRate __SetBaudRate;
    using ptrToSetDataCharacteristics = FT_STATUS(WINAPI *)(FT_HANDLE, UCHAR, UCHAR, UCHAR);
    ptrToSetDataCharacteristics __SetDataCharacteristics;
    using ptrToPurge = FT_STATUS(WINAPI *)(FT_HANDLE, ULONG);
    ptrToPurge __Purge;

public:
    FTD2(const char *lib_path);
    ~FTD2() {}
    FT_STATUS Open(int dev_idx, FT_HANDLE *hdl);
    FT_STATUS Open(const std::string &dev_desc, uint32_t flags, FT_HANDLE *hdl);
    FT_STATUS Write(FT_HANDLE hdl, uint8_t *buf, uint32_t size_buf, uint32_t *size_write);
    FT_STATUS Read(FT_HANDLE hdl, uint8_t *buf, uint32_t size_buf, uint32_t *size_read);
    FT_STATUS GetQueueStatus(FT_HANDLE hdl, uint32_t *size_buf);
    FT_STATUS SetBaudRate(FT_HANDLE hdl, uint32_t BaudRate);
    FT_STATUS SetDataCharacteristics(FT_HANDLE hdl, uint8_t WordLength, uint8_t StopBits, uint8_t Parity);
    FT_STATUS Purge(FT_HANDLE hdl, uint32_t Mask);
};
class FTD3 : public FTD_SERIES
{
protected:
    using ptrToCreate = FT_STATUS(WINAPI *)(PVOID, DWORD, FT_HANDLE *);
    ptrToCreate __Create;
    using ptrToWritePipe = FT_STATUS(WINAPI *)(FT_HANDLE, UCHAR, PUCHAR, ULONG, PULONG, LPOVERLAPPED);
    ptrToWritePipe __WritePipe;
    using ptrToReadPipe = FT_STATUS(WINAPI *)(FT_HANDLE, UCHAR, PUCHAR, ULONG, PULONG, LPOVERLAPPED);
    ptrToReadPipe __ReadPipe;

public:
    FTD3(const char *lib_path);
    ~FTD3() {}
    FT_STATUS Create(const std::string &dev_desc, uint32_t flags, FT_HANDLE *hdl);
    FT_STATUS Write(FT_HANDLE hdl, uint8_t *buf, uint32_t size_buf, uint32_t *size_write);
    FT_STATUS Read(FT_HANDLE hdl, uint8_t *buf, uint32_t size_buf, uint32_t *size_read);
};

FTD_SERIES::FTD_SERIES(const char *lib_path)
{
    lib = LoadLibrary(TEXT(lib_path));
    if (lib == NULL)
    {
        auto error = GetLastError();
        std::cout << "ERROR: " << error << std::endl;
    }
    __ListDevices = (ptrToListDevices)GetProcAddress(lib, TEXT("FT_ListDevices"));
    __Close = (ptrToClose)GetProcAddress(lib, TEXT("FT_Close"));
    __ResetDevice = (ptrToResetDevice)GetProcAddress(lib, TEXT("FT_ResetDevice"));
}
FTD_SERIES::~FTD_SERIES()
{
    if (lib != NULL)
        FreeLibrary(lib);
    lib = NULL;
}
FTD2::FTD2(const char *lib_path) : FTD_SERIES::FTD_SERIES(lib_path)
{
    __Open = (ptrToOpen)GetProcAddress(lib, TEXT("FT_Open"));
    __OpenEx = (ptrToOpenEx)GetProcAddress(lib, TEXT("FT_OpenEx"));
    __Write = (ptrToWrite)GetProcAddress(lib, TEXT("FT_Write"));
    __Read = (ptrToRead)GetProcAddress(lib, TEXT("FT_Read"));
    __GetQueueStatus = (ptrToGetQueueStatus)GetProcAddress(lib, TEXT("FT_GetQueueStatus"));
    __SetBaudRate = (ptrToSetBaudRate)GetProcAddress(lib, TEXT("FT_SetBaudRate"));
    __SetDataCharacteristics = (ptrToSetDataCharacteristics)GetProcAddress(lib, TEXT("FT_SetDataCharacteristics"));
    __Purge = (ptrToPurge)GetProcAddress(lib, TEXT("FT_Purge"));
}
FTD3::FTD3(const char *lib_path) : FTD_SERIES::FTD_SERIES(lib_path)
{
    __Create = (ptrToCreate)GetProcAddress(lib, TEXT("FT_Create"));
    __WritePipe = (ptrToWritePipe)GetProcAddress(lib, TEXT("FT_WritePipe"));
    __ReadPipe = (ptrToReadPipe)GetProcAddress(lib, TEXT("FT_ReadPipe"));
}
std::vector<std::string> FTD_SERIES::ListDevices()
{
    std::vector<std::string> out;
    FT_STATUS ftStatus;
    DWORD numDevs;
    char *BufPtrs[64];

    if (!__ListDevices)
    {
        std::cout << "*** FT_ListDevices is not valid!" << std::endl;
        return out;
    }
    ftStatus = __ListDevices(&numDevs, NULL, FT_LIST_NUMBER_ONLY);
    if (ftStatus == FT_OK)
    {
        for (DWORD u = 0; u < numDevs; u++)
            BufPtrs[u] = new char[64]{0};
        BufPtrs[numDevs] = NULL;
        ftStatus = __ListDevices(BufPtrs, &numDevs, FT_LIST_ALL | FT_OPEN_BY_DESCRIPTION);
        for (DWORD u = 0; u < numDevs; u++)
            out.push_back(BufPtrs[u]);
        for (DWORD u = 0; u < numDevs; u++)
            delete[] BufPtrs[u];
    }
    else
    {
        std::cout << "*** Fail to run FT_ListDevices " << std::endl;
    }
    return out;
}
FT_STATUS FTD_SERIES::Close(FT_HANDLE hdl)
{
    if (!__Close)
    {
        std::cout << "*** FT_Close is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __Close(hdl);
}
FT_STATUS FTD_SERIES::ResetDevice(FT_HANDLE hdl)
{
    if (!__ResetDevice)
    {
        std::cout << "*** FT_ResetDevice is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __ResetDevice(hdl);
}
FT_STATUS FTD2::Open(int dev_idx, FT_HANDLE *handle)
{
    if (!__Open)
    {
        std::cout << "*** FT_Open is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __Open(dev_idx, handle);
}
FT_STATUS FTD2::Open(const std::string &dev_desc, uint32_t flags, FT_HANDLE *hdl)
{
    if (!__OpenEx)
    {
        std::cout << "*** FT_OpenEx is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __OpenEx((PVOID)dev_desc.c_str(), (DWORD)flags, hdl);
}
FT_STATUS FTD2::Write(FT_HANDLE hdl, uint8_t *buf, uint32_t size_buf, uint32_t *size_write)
{
    if (!__Write)
    {
        std::cout << "*** FT_Write is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __Write(hdl, (LPVOID)buf, (DWORD)size_buf, (LPDWORD)size_write);
}
FT_STATUS FTD2::Read(FT_HANDLE hdl, uint8_t *buf, uint32_t size_buf, uint32_t *size_read)
{
    if (!__Read)
    {
        std::cout << "*** FT_Read is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __Read(hdl, (LPVOID)buf, (DWORD)size_buf, (LPDWORD)size_read);
}
FT_STATUS FTD2::GetQueueStatus(FT_HANDLE hdl, uint32_t *size_buf)
{
    if (!__GetQueueStatus)
    {
        std::cout << "*** FT_GetQueueStatus is not valid!" << endl;
        return FT_INVALID_HANDLE;
    }
    return __GetQueueStatus(hdl, (DWORD *)size_buf);
}
FT_STATUS FTD2::SetBaudRate(FT_HANDLE hdl, uint32_t BaudRate)
{
    if (!__SetBaudRate)
    {
        std::cout << "*** FT_SetBaudRate is not valid!" << endl;
        return FT_INVALID_HANDLE;
    }
    return __SetBaudRate(hdl, (ULONG)BaudRate);
}
FT_STATUS FTD2::SetDataCharacteristics(FT_HANDLE hdl, uint8_t WordLength, uint8_t StopBits, uint8_t Parity)
{
    if (!__SetDataCharacteristics)
    {
        std::cout << "*** FT_SetDataCharacteristics is not valid!" << endl;
        return FT_INVALID_HANDLE;
    }
    return __SetDataCharacteristics(hdl, WordLength, StopBits, Parity);
}
FT_STATUS FTD2::Purge(FT_HANDLE hdl, uint32_t Mask)
{
    if (!__Purge)
    {
        std::cout << "*** FT_Purge is not valid!" << endl;
        return FT_INVALID_HANDLE;
    }
    return __Purge(hdl, Mask);
}
FT_STATUS FTD3::Create(const std::string &dev_desc, uint32_t flags, FT_HANDLE *hdl)
{
    if (!__Create)
    {
        std::cout << "*** FT_Create is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __Create((PVOID)dev_desc.c_str(), (DWORD)flags, hdl);
}
FT_STATUS FTD3::Write(FT_HANDLE hdl, uint8_t *buf, uint32_t size_buf, uint32_t *size_write)
{
    if (!__WritePipe)
    {
        std::cout << "*** FT_WritePipe is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __WritePipe(hdl, 0x02, (PUCHAR)buf, (ULONG)size_buf, (PULONG)size_write, NULL);
}
FT_STATUS FTD3::Read(FT_HANDLE hdl, uint8_t *buf, uint32_t size_buf, uint32_t *size_read)
{
    if (!__ReadPipe)
    {
        std::cout << "*** FT_ReadPipe is not valid!";
        return FT_INVALID_HANDLE;
    }
    return __ReadPipe(hdl, 0x82, (PUCHAR)buf, (ULONG)size_buf, (PULONG)size_read, NULL);
}

class PM1K
{
private:
    // FTD3 comm{"FTD3XX.dll"};
    FTD3 *comm;
    FT_HANDLE hdl = (FT_HANDLE)NULL;

    void Write(uint16_t addr, uint16_t data);
    uint16_t Read(uint16_t addr);
    uint16_t crc(uint16_t *pack, uint32_t length, uint16_t crc_init);
    std::vector<uint16_t> _s;

public:
    PM1K() {}
    ~PM1K();
    int Init();
    void Term();
    std::vector<uint16_t> ReadSOP();
    std::vector<float> ReadSOP_R();
    void WriteSOP();
    void WriteSOP(const std::vector<uint16_t> &sop);
    void WriteSOP(const std::vector<float> &sop);
    void AlignSOP();
};
PM1K::~PM1K()
{
    if (hdl)
        Term();
}
int PM1K::Init()
{
    comm = new FTD3("FTD3XX.dll");

    auto list = comm->ListDevices();
    for (const auto &s : list)
        if (s.find("PM1000") != std::string::npos)
        {
            comm->Create(s, FT_OPEN_BY_DESCRIPTION, &hdl);
            kpa::ins::msg.prefix("Instrument Name : ") << s;
            break;
        }
    if (hdl == (FT_HANDLE)NULL)
    {
        std::cout << "*** Fail to connect instrument PM1000!" << std::endl;
        return -1;
    }
    else
    {
        return 0;
    }
}
void PM1K::Term()
{
    if (hdl == (FT_HANDLE)NULL)
        return;
    auto state = comm->Close(hdl);
    if (state == FT_OK)
    {
        delete comm;
        comm = NULL;
        hdl = (FT_HANDLE)NULL;
    }
    else
    {
        std::cout << "*** Fail to disconnect PM1000 !";
    }
}
uint16_t PM1K::crc(uint16_t *pack, uint32_t length = 3, uint16_t crc_init = 0xFFFF)
{
    uint16_t crc = crc_init;
    while (length--)
    {
        crc = (crc << 1) | (crc >> 15); // cyclic shift of crc
        crc ^= *(pack++);               // xor with data
    }
    return crc;
}
void PM1K::Write(uint16_t addr, uint16_t data)
{
    uint32_t bytes;
    uint16_t pack[4] = {'W', addr, data, 0}, rdpack[4] = {0};
    pack[3] = crc(pack, 3);

    bytes = 0;
    comm->Write(hdl, (uint8_t *)pack, 8, &bytes);
    bytes = 0;
    comm->Read(hdl, (uint8_t *)rdpack, 8, &bytes);
    if (rdpack[0] != 52428)
        std::cout << "*** CRC ERROR during PM1K:Write!";
    return;
}
uint16_t PM1K::Read(uint16_t addr)
{
    uint32_t bytes;
    uint16_t pack[4] = {'R', addr, 0, 0}, rdpack[4] = {0};
    pack[3] = crc(pack);

    bytes = 0;
    comm->Write(hdl, (uint8_t *)pack, 8, &bytes);
    // msg.prefix("wt_bytes: ") << bytes;
    bytes = 0;
    comm->Read(hdl, (uint8_t *)rdpack, 8, &bytes);
    // msg.prefix("rd_bytes: ") << bytes;
    // msg.format("0x%0X") << vector<uint16_t>(rdpack, rdpack + 4);
    if (crc(rdpack + 2, 1, pack[3]) != rdpack[1])
    {
        std::cout << "*** CRC ERROR during PM1K:Read!" << std::endl;
        return 0;
    }
    return rdpack[2];
}
std::vector<uint16_t> PM1K::ReadSOP()
{
    if (_s.size() != 3)
        _s.resize(3);
    //* read SOP (PM1000) - standard normalization
    _s[0] = Read(512 + 25);
    _s[1] = Read(512 + 26);
    _s[2] = Read(512 + 27);
    return _s;
}
std::vector<float> PM1K::ReadSOP_R()
{
    const float h16b = 1 << 15;
    auto buf = ReadSOP();
    std::vector<float> out(3, 0.0);

    for (int i = 0; i < (int)out.size(); i++)
        out[i] = (buf[i] / h16b) - 1; //* R = D/(2^15)-1
    return out;
}
void PM1K::WriteSOP(const std::vector<uint16_t> &sop)
{
    if (sop.size() != 3)
        return;
    //* write target SOP (PM1000)
    Write(512 + 87, sop[0]);
    Write(512 + 88, sop[1]);
    Write(512 + 89, sop[2]);
    return;
}
void PM1K::WriteSOP(const std::vector<float> &sop)
{
    auto round = [](float in) -> uint16_t
    {
        if (in >= 65535.0)
            return 65535;
        return uint16_t(in + 0.5);
    };
    const float h16b = 1 << 15;
    std::vector<uint16_t> buf(3, 0);

    for (int i = 0; i < (int)buf.size(); i++)
        buf[i] = round((sop[i] + 1.0) * h16b); //*  D = (R+1)*(2^15)
    WriteSOP(buf);
}
void PM1K::WriteSOP()
{
    if (_s.size() != 3)
        ReadSOP();
    WriteSOP(_s);
}
void PM1K::AlignSOP()
{
    //* read SOP (PM1000) - standard normalization
    auto s1 = Read(512 + 25);
    auto s2 = Read(512 + 26);
    auto s3 = Read(512 + 27);
    //* write target SOP (PM1000)
    Write(512 + 87, kpa::ins::msg.prefix("wt_s1: ") << s1);
    Write(512 + 88, kpa::ins::msg.prefix("wt_s2: ") << s2);
    Write(512 + 89, kpa::ins::msg.prefix("wt_s3: ") << s3);

    // msg.prefix("rd_s1: ") << Read(512 + 87);
    // msg.prefix("rd_s2: ") << Read(512 + 88);
    // msg.prefix("rd_s3: ") << Read(512 + 89);
}

class EPS1000
{
protected:
    FT_HANDLE hdl = (FT_HANDLE)NULL;
    FTD2 *comm;
    bool __fb_en = false;

    char subBit4ToChar(uint16_t bits);
    uint16_t subCharToBit4(char ch);
    void write(uint16_t addr, uint16_t data);
    uint16_t read(uint16_t addr);

public:
    enum MODE
    {
        last_setting = 0,
        digital,
        analog,
        analog_highloss
    };
    enum TRACE
    {
        IL_MAX = 0,
        IL_MIN
    };

    EPS1000(){};
    ~EPS1000();
    int Init();
    int Term();
    void feedback(bool enable, EPS1000::MODE mode = last_setting, EPS1000::TRACE trace = IL_MIN);
};
EPS1000::~EPS1000()
{
    if (hdl)
        Term();
}
int EPS1000::Init()
{
    // FT_STATUS state;
    if (hdl)
        return -2;
    comm = new FTD2("ftd2xx.dll");
    // comm = new FTD2("C:\\KPA2_PROJECT_X64\\.head\\ftd2xx.dll");

    auto list = comm->ListDevices();
    for (const auto &s : list)
        if (s.find("EPS1000") != std::string::npos)
        {
            comm->Open(s, FT_OPEN_BY_DESCRIPTION, &hdl);
            kpa::ins::msg.prefix("Instrument Name : ") << s;
            break;
        }
    if (hdl == (FT_HANDLE)NULL)
    {
        std::cout << "*** Fail to connect instrument EPS1000!" << std::endl;
        return -1;
    }
    comm->SetBaudRate(hdl, 230400);
    comm->SetDataCharacteristics(hdl, 8, 0, 0);
    comm->Purge(hdl, FT_PURGE_RX | FT_PURGE_TX);
    return 0;
}
int EPS1000::Term()
{
    if (!hdl)
        return -2;
    auto state = comm->Close(hdl);
    if (state == FT_OK)
    {
        delete comm;
        comm = NULL;
        hdl = (FT_HANDLE)NULL;
    }
    else
    {
        std::cout << "*** Fail to disconnect EPS1000 !";
        return -1;
    }
    return 0;
}
char EPS1000::subBit4ToChar(uint16_t bits)
{
    uint8_t dig = bits & 0x0F;
    if (dig < 10)
        return dig + '0';
    else if (dig < 16)
        return dig - 10 + 'A';
    return '0';
};
uint16_t EPS1000::subCharToBit4(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    return 0;
};
void EPS1000::write(uint16_t addr, uint16_t data)
{
    uint32_t bytes = 0;
    char buf[10] = "W0000000\r";

    if (!hdl)
        return;
    buf[1] = subBit4ToChar(addr >> 8);
    buf[2] = subBit4ToChar(addr >> 4);
    buf[3] = subBit4ToChar(addr >> 0);
    buf[4] = subBit4ToChar(data >> 12);
    buf[5] = subBit4ToChar(data >> 8);
    buf[6] = subBit4ToChar(data >> 4);
    buf[7] = subBit4ToChar(data >> 0);
    comm->Write(hdl, (uint8_t *)buf, 9, &bytes);
    return;
}
uint16_t EPS1000::read(uint16_t addr)
{
    kpa::ins::msg << "EPS1000::READ >> ";
    kpa::ins::MSG_INDENT __t;
    uint32_t bytes = 0;
    char buf[10] = "R0000000\r";
    FT_STATUS state;

    if (!hdl)
        return 0;
    buf[1] = subBit4ToChar(addr >> 8);
    buf[2] = subBit4ToChar(addr >> 4);
    buf[3] = subBit4ToChar(addr >> 0);
    state = comm->Write(hdl, (uint8_t *)buf, 9, &bytes);
    if (state != FT_OK)
        return 0;

    uint32_t buf_r_size = 0;
    for (int ms = 0; ms < 5000; ms++)
    {
        Sleep(1);
        state = comm->GetQueueStatus(hdl, &buf_r_size);
        if (state != FT_OK)
            kpa::ins::msg << (int)state;
        if (buf_r_size >= 5)
            break;
    }

    bytes = 0;
    char *buf_r = buf + 4;
    uint16_t out = 0;
    state = comm->Read(hdl, (uint8_t *)buf_r, buf_r_size, &bytes);
    if (state != FT_OK)
        return 0;
    kpa::ins::msg.prefix("EPS1000 reg_read: ") << buf;
    kpa::ins::msg.prefix("Receive bytes: ") << (int)bytes;
    out |= subCharToBit4(buf_r[0]) << 12;
    out |= subCharToBit4(buf_r[1]) << 8;
    out |= subCharToBit4(buf_r[2]) << 4;
    out |= subCharToBit4(buf_r[3]);

    return out;
}
void EPS1000::feedback(bool enable, EPS1000::MODE mode, EPS1000::TRACE trace)
{
    int err_sgn_delay, ate, dither_amp;
    int max_fb;
    union ADDR347
    {
        struct
        {
            uint16_t detector_mode : 2, gradient_type : 1, reference_path : 1, sensitive_mode : 1, : 1, APD_control : 1;
        };
        uint16_t reg = 0;
    } reg347;

    if (!hdl)
        return;
    if (mode != last_setting)
    {
        switch (mode)
        {
        case digital: //! digital feedback
            reg347.detector_mode = 0;
            err_sgn_delay = 41;
            ate = 2;
            dither_amp = 2;
            reg347.gradient_type = 0;
            // max_fb = trace; //! for ATE=7
            max_fb = 0; //! for ATE=3
            break;
        case analog_highloss:
            reg347.detector_mode = 2;
            err_sgn_delay = 300;
            ate = 10;
            dither_amp = 2;
            reg347.gradient_type = 1;
            max_fb = trace;
            break;
        case analog: //! analog feedback
        default:
            reg347.detector_mode = 2;
            err_sgn_delay = 300;
            ate = 7;
            dither_amp = 2;
            reg347.gradient_type = 1;
            max_fb = trace;
            break;
        }
        reg347.APD_control = 0;
        reg347.sensitive_mode = 0;
        reg347.reference_path = 0;
        write(347, reg347.reg);    //
        Sleep(1);                  //
        write(342, max_fb);        //! minimization / maximization
        Sleep(1);                  //
        write(344, err_sgn_delay); //! Error signal delay
        Sleep(1);                  //
        write(343, ate);           //! Averaging time exponent
        Sleep(1);                  //
        write(345, dither_amp);    //! Dither amplitude
        Sleep(1);                  //
    }
    else
    {
        Sleep(100);
    }
    // msg.prefix("Polar Feedback Mode: ") << enable;
    for (int i = 0; i < 3; i++)
    {
        write(340, (enable) ? 3 : 2); //! Enabling/disabling tracking mode
        Sleep(10);
    }
}
