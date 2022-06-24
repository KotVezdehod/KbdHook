#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#include "include\ComponentBase.h"
#include "include\AddInDefBase.h"
#include "include\IMemoryManager.h"
#include <locale>
#include <stdio.h>
#include <string>

#include <codecvt>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <thread>

void ToV8String(const wchar_t* wstr, tVariant* par, IMemoryManager* m_iMemory);
long numericValue(tVariant* par);
bool isNumericParameter(tVariant* par);
int mOpenSymbol = 0;
int mCloseSymbol = 0;
int mInterceptorType = -1;

static const wchar_t* g_PropNames[] = {
    L"Epty"
};
static const wchar_t* g_MethodNames[] = {
    L"SetHook",
    L"UnsetHook"
};

static const wchar_t* g_PropNamesRu[] = {
    L"ѕустой"
};

static const wchar_t* g_MethodNamesRu[] = {
    L"”становитьѕерехват",
    L"—н€тьѕерехват"
};

class KeyDescription
{


public:

    KeyDescription(int in_vc, int in_sc, std::wstring in_key);
    ~KeyDescription();

    int Vc;
    int Sc;
    std::wstring Key_;


private:


};
std::vector<KeyDescription> key_queue;

///////////////////////////////////////////////////////////////////////////////
// class CAddInNative
class CAddInNative : public IComponentBase
{
public:
    enum Props
    {
        ePropLast      // Always last
    };

    enum Methods
    {
        eMethSetHook,
        eMethUnsetHook,
        eMethLast      // Always last
    };

    CAddInNative(void);
    virtual ~CAddInNative();
    // IInitDoneBase
    virtual bool ADDIN_API Init(void*);
    virtual bool ADDIN_API setMemManager(void* mem);
    virtual long ADDIN_API GetInfo();
    virtual void ADDIN_API Done();
    // ILanguageExtenderBase
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T**);
    virtual long ADDIN_API GetNProps();
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal);
    virtual bool ADDIN_API IsPropReadable(const long lPropNum);
    virtual bool ADDIN_API IsPropWritable(const long lPropNum);
    virtual long ADDIN_API GetNMethods();
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum,
        const long lMethodAlias);
    virtual long ADDIN_API GetNParams(const long lMethodNum);
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum,
        tVariant* pvarParamDefValue);
    virtual bool ADDIN_API HasRetVal(const long lMethodNum);
    virtual bool ADDIN_API CallAsProc(const long lMethodNum,
        tVariant* paParams, const long lSizeArray);
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum,
        tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    // LocaleBase
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc);

    IAddInDefBase* m_iConnect;
    IMemoryManager* m_iMemory;
    

private:
    long findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const;
    void addError(uint32_t wcode, const wchar_t* source,
        const wchar_t* descriptor, long code);
    // Attributes

};

class WcharWrapper
{
public:
    WcharWrapper(const wchar_t* str);
    ~WcharWrapper();
    operator const wchar_t* () { return m_str_wchar; }
    operator wchar_t* () { return m_str_wchar; }
private:
    WcharWrapper& operator = (const WcharWrapper& other) { return *this; }
    WcharWrapper(const WcharWrapper& other) { }
private:
    wchar_t* m_str_wchar;
};


#endif //__ADDINNATIVE_H__
