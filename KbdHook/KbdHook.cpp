#include "pch.h"
#include "../KbdHook/include/KbdHook.h"
#include "../KbdHook/include/Json.h"

using namespace std;

#define TIME_LEN 65

#define BASE_ERRNO     7

#ifdef WIN32
#pragma setlocale("ru-RU" )
#endif

#pragma region HookRegion
#define _WIN32_WINNT 0x0400
#pragma comment( lib, "user32.lib" )
HHOOK hKeyboardHook = nullptr;
CAddInNative* m_CaddInNative = nullptr;
typedef  BOOL(WINAPI KEYBOARDEVENT)(UINT);
KEYBOARDEVENT* KeyboardEventProc = nullptr;
HINSTANCE mInstance = nullptr;
#pragma endregion


static const wchar_t g_kClassNames[] = L"CAddInNative"; //"|OtherClass1|OtherClass2";
static IAddInDefBase* pAsyncEvent = NULL;


uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, size_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static WcharWrapper s_names(g_kClassNames);

//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if (!*pInterface)
    {
        *pInterface = new CAddInNative;
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
    if (!*pIntf)
        return -1;

    delete* pIntf;
    *pIntf = 0;
    return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    return s_names;
}
//---------------------------------------------------------------------------//

// CAddInNative
//---------------------------------------------------------------------------//
CAddInNative::CAddInNative()
{
    m_iMemory = 0;
    m_iConnect = 0;
    m_CaddInNative = this;

    wchar_t path[MAX_PATH];

    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&KeyboardEventProc, &mInstance))
    {
        if (mInstance)
        {
            KeyboardEventProc = (KEYBOARDEVENT*)::GetProcAddress(mInstance, "KeyboardEvent");
        }
    }
    
    
}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
    m_CaddInNative = nullptr;
}
//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{
    m_iConnect = (IAddInDefBase*)pConnection;
    return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{
    // Component should put supported component technology version 
    // This component supports 2.0 version
    return 2000;
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{

}
/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{
    const wchar_t* wsExtension = L"KbdHook";
    uint32_t iActualSize = static_cast<uint32_t>(::wcslen(wsExtension) + 1);

    if (m_iMemory)
    {
        if (m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
        {
            convToShortWchar(wsExtensionName, wsExtension, iActualSize);
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{
    // You may delete next lines and add your own implementation code here
    return ePropLast;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{
    long plPropNum = -1;
    wchar_t* propName = 0;

    ::convFromShortWchar(&propName, wsPropName);
    plPropNum = findName(g_PropNames, propName, ePropLast);

    if (plPropNum == -1)
        plPropNum = findName(g_PropNamesRu, propName, ePropLast);

    delete[] propName;

    return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{
    if (lPropNum >= ePropLast)
        return NULL;

    wchar_t* wsCurrentName = NULL;
    WCHAR_T* wsPropName = NULL;
    size_t iActualSize = 0;

    switch (lPropAlias)
    {
    case 0: // First language
        wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
        break;
    case 1: // Second language
        wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
        break;
    default:
        return 0;
    }

    iActualSize = wcslen(wsCurrentName) + 1;

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsPropName, (unsigned)iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
    }

    return wsPropName;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{
    switch (lPropNum)
    {
        /*case ePropIsEnabled:
            TV_VT(pvarPropVal) = VTYPE_BOOL;
            TV_BOOL(pvarPropVal) = m_boolEnabled;
            break;
        case ePropIsTimerPresent:
            TV_VT(pvarPropVal) = VTYPE_BOOL;
            TV_BOOL(pvarPropVal) = true;
            break;*/
    default:
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{
    switch (lPropNum)
    {
        /*case ePropIsEnabled:
            if (TV_VT(varPropVal) != VTYPE_BOOL)
                return false;
            m_boolEnabled = TV_BOOL(varPropVal);
            break;
        case ePropIsTimerPresent:*/
    default:
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{
    switch (lPropNum)
    {
        /*case ePropIsEnabled:
        case ePropIsTimerPresent:
            return true;*/
    default:
        return false;
    }

    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
    switch (lPropNum)
    {
        /* case ePropIsEnabled:
             return true;
         case ePropIsTimerPresent:
             return false;*/
    default:
        return false;
    }

    return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{
    return eMethLast;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{
    long plMethodNum = -1;
    wchar_t* name = 0;

    ::convFromShortWchar(&name, wsMethodName);

    plMethodNum = findName(g_MethodNames, name, eMethLast);

    if (plMethodNum == -1)
        plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

    delete[] name;

    return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias)
{
    if (lMethodNum >= eMethLast)
        return NULL;

    wchar_t* wsCurrentName = NULL;
    WCHAR_T* wsMethodName = NULL;
    size_t iActualSize = 0;

    switch (lMethodAlias)
    {
    case 0: // First language
        wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
        break;
    case 1: // Second language
        wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
        break;
    default:
        return 0;
    }

    iActualSize = wcslen(wsCurrentName) + 1;

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsMethodName, (unsigned)iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
    }

    return wsMethodName;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{
    switch (lMethodNum)
    {

    case eMethSetHook:
        return 3;

    default:
        break;
    }

    return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
    tVariant* pvarParamDefValue)
{

    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{
    switch (lMethodNum)
    {
    case eMethSetHook:
    case eMethUnsetHook:
        return true;
    default:
        return false;
    }

}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
    tVariant* paParams, const long lSizeArray)
{
    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
    tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    switch (lMethodNum)
	{
	case eMethSetHook:
	{
		
		if (mInstance)
		{
			if (!hKeyboardHook)
			{
				if (KeyboardEventProc)
				{

                    mOpenSymbol = 0;
                    mCloseSymbol = 0;
                    mInterceptorType = -1;
                    key_queue.clear();

                    int loc_open_symbol = 0;
                    int loc_close_symbol = 0;
                    int loc_interceptor_type = -1;

                    if (lSizeArray < 3)
                    {
                        ToV8String(L"Входных параметров должно быть 3 (тип перехватчика ['eachsymbol'|'string'], ОткрывающийКлюч, ЗакрывающийКлюч) ", pvarRetValue, m_iMemory);
                        return true;
                    }

                    if (TV_VT(&paParams[0]) != VTYPE_PWSTR)
                    {
                        ToV8String(L"Параметр 1 должен быть строчным.", pvarRetValue, m_iMemory);
                        return true;
                    }
                    if (!isNumericParameter(&paParams[1]))
                    {
                        ToV8String(L"Параметр 2 должен быть целым, не отрицательным числом в диапазоне 1-65535.", pvarRetValue, m_iMemory);
                        return true;
                    }
                    else
                    {
                        loc_open_symbol = numericValue(&paParams[1]);
                        if (loc_open_symbol < 1 || loc_open_symbol > 65535)
                        {
                            ToV8String(L"Параметр 2 должен быть целым, не отрицательным числом в диапазоне 1-65535.", pvarRetValue, m_iMemory);
                            return true;
                        }
                    }

                    if (!isNumericParameter(&paParams[2]))
                    {
                        ToV8String(L"Параметр 3 должен быть целым, не отрицательным числом в диапазоне 1-65535.", pvarRetValue, m_iMemory);
                        return true;
                    }
                    else
                    {
                        loc_close_symbol = numericValue(&paParams[2]);
                        if (loc_close_symbol < 1 || loc_close_symbol > 65535)
                        {
                            ToV8String(L"Параметр 3 должен быть целым, не отрицательным числом в диапазоне 1-65535.", pvarRetValue, m_iMemory);
                            return true;
                        }
                    }

                    wchar_t* wch_interceptor_type = nullptr;

                    convFromShortWchar(&wch_interceptor_type, (&paParams[0])->pwstrVal);
                    if (!wcscmp(wch_interceptor_type, L"char"))
                    {
                        loc_interceptor_type = 0;
                    }
                    else if (!wcscmp(wch_interceptor_type, L"string"))
                    {
                        loc_interceptor_type = 1;
                    }

                    delete[] wch_interceptor_type;

                    if (loc_interceptor_type < 0)
                    {
                        wstring ws_err = L"Не известный тип перехвата (параметр 1 должен быть 'char' или 'string')";
                        ToV8String(ws_err.c_str(), pvarRetValue, m_iMemory);
                        return true;
                    }

					if (loc_interceptor_type == 1)
					{
                        mOpenSymbol = loc_open_symbol;
                        mCloseSymbol = loc_close_symbol;
                        mInterceptorType = loc_interceptor_type;
					}
					

					hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardEventProc, mInstance, NULL);

					if (!hKeyboardHook)
					{
						int err = GetLastError();
						wstring ws_err = L"Перехват установить не удалось: " + to_wstring(err);

						ToV8String(ws_err.c_str(), pvarRetValue, m_iMemory);
					}
					else
					{

						ToV8String(L"ok", pvarRetValue, m_iMemory);
					}
				}
				else
				{
					ToV8String(L"Не удалось получить точку входа процедуры-перехватчика.", pvarRetValue, m_iMemory);
				}
			}
			else
			{
				ToV8String(L"Перехват уже установлен.", pvarRetValue, m_iMemory);
			}
		}
		else
		{
			ToV8String(L"Не удалось получить дескриптор динамической библиотеки.", pvarRetValue, m_iMemory);
		}
	}
	return true;

	case eMethUnsetHook:

        if (hKeyboardHook)
        {
            UnhookWindowsHookEx(hKeyboardHook);
            mOpenSymbol = 0;
            mCloseSymbol = 0;
            mInterceptorType = -1;
            key_queue.clear();
            ToV8String(L"ok", pvarRetValue, m_iMemory);
        }
        else
        {
            ToV8String(L"Перехват не установлен.", pvarRetValue, m_iMemory);
        }

        hKeyboardHook = nullptr;

        return true;

    default:
        return false;
    }

    return true;
}


//---------------------------------------------------------------------------//
void CAddInNative::SetLocale(const WCHAR_T* loc)
{
   _wsetlocale(LC_ALL, loc);
}
/////////////////////////////////////////////////////////////////////////////
// LocaleBase
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
void CAddInNative::addError(uint32_t wcode, const wchar_t* source,
    const wchar_t* descriptor, long code)
{
    if (m_iConnect)
    {
        WCHAR_T* err = 0;
        WCHAR_T* descr = 0;

        ::convToShortWchar(&err, source);
        ::convToShortWchar(&descr, descriptor);

        m_iConnect->AddError(wcode, err, descr, code);
        delete[] err;
        delete[] descr;
    }
}
//---------------------------------------------------------------------------//
long CAddInNative::findName(const wchar_t* names[], const wchar_t* name,
    const uint32_t size) const
{
    long ret = -1;
    for (uint32_t i = 0; i < size; i++)
    {
        if (!wcscmp(names[i], name))
        {
            ret = i;
            break;
        }
    }
    return ret;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, size_t len)
{
    if (!len)
        len = ::wcslen(Source) + 1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(WCHAR_T));
    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpShort = (WCHAR_T)*tmpWChar;
    }

    return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source) + 1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(wchar_t));
    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpWChar = (wchar_t)*tmpShort;
    }

    return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
WcharWrapper::WcharWrapper(const wchar_t* str) :
    m_str_wchar(NULL)
{
    if (str)
    {
        size_t len = wcslen(str);
        m_str_wchar = new wchar_t[len + 1];
        memset(m_str_wchar, 0, sizeof(wchar_t) * (len + 1));
        memcpy(m_str_wchar, str, sizeof(wchar_t) * len);
    }

}
//---------------------------------------------------------------------------//
WcharWrapper::~WcharWrapper()
{
    if (m_str_wchar)
    {
        delete[] m_str_wchar;
        m_str_wchar = NULL;
    }
}
//---------------------------------------------------------------------------//

void ToV8String(const wchar_t* wstr, tVariant* par, IMemoryManager* m_iMemory)
{
    if (wstr)
    {
        ULONG len = wcslen(wstr);
        m_iMemory->AllocMemory((void**)&par->pwstrVal, (len + 1) * sizeof(WCHAR_T));
        convToShortWchar(&par->pwstrVal, wstr);
        par->vt = VTYPE_PWSTR;
        par->wstrLen = len;
    }
    else
        par->vt = VTYPE_EMPTY;
}

long numericValue(tVariant* paParams)
{
    long ret = 0;
    switch (paParams->vt)
    {
    case VTYPE_I4:
        ret = paParams->lVal;
        break;
    case VTYPE_UI4:
        ret = paParams->ulVal;
        break;
    case VTYPE_R8:
        ret = paParams->dblVal;
        break;
    case VTYPE_R4:
        ret = paParams->dblVal;
        break;
    }
    return ret;
}

bool isNumericParameter(tVariant* par)
{
    return par->vt == VTYPE_I4 || par->vt == VTYPE_UI4 || par->vt == VTYPE_R8;
}

#pragma region HookRegion

static const wchar_t g_EventSource[] = L"KbdHook";
static WcharWrapper s_EventSource(g_EventSource);

static const wchar_t g_EventKeyHooked[] = L"KeyHooked";
static WcharWrapper s_EventNameKeyHooked(g_EventKeyHooked);

LRESULT CALLBACK KeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (m_CaddInNative)
    {
		DWORD SHIFT_key = 0;
		DWORD CTRL_key = 0;
		DWORD ALT_key = 0;

		if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)))
		{
			KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
			DWORD dwMsg = 1;
			dwMsg += hooked_key.scanCode << 16;
			dwMsg += hooked_key.flags << 24;

			wchar_t tmp_wch[32];
			memset(&tmp_wch, 0, sizeof(wchar_t)*32);
			int i = GetKeyNameText(dwMsg, tmp_wch, 0x20);

			if (!(wcscmp(tmp_wch, L"Ctrl") == 0 || wcscmp(tmp_wch, L"Shift") == 0 || wcscmp(tmp_wch, L"Alt") == 0))
			{

				if (mInterceptorType==1)
				{
					unsigned char state[256];
					GetKeyboardState(state);

					wchar_t unicode[32];
					memset(unicode, 0, sizeof(wchar_t) * 32);
					UINT ascii_code = ToUnicodeEx(hooked_key.vkCode, hooked_key.scanCode, state, unicode, 2, 0, GetKeyboardLayout(0));

					if (key_queue.size())
					{
						if (key_queue[key_queue.size() - 1].Sc != mCloseSymbol)         //parallel thread is active - idle all actions for now...
						{
							key_queue.push_back(KeyDescription(hooked_key.vkCode, hooked_key.scanCode, wstring(unicode)));
							if (hooked_key.scanCode == mCloseSymbol)
							{
								//Send to 1c and clear queue

								thread t_0([](UINT ascii_code, wstring t0_unicode, KBDLLHOOKSTRUCT t0_hooked_key)
									{

										wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
										Json::Value root;

										for (KeyDescription& kd : key_queue)
										{
											Json::Value KeyDescr;
											KeyDescr["vk"] = kd.Vc;
											KeyDescr["sc"] = kd.Sc;
											KeyDescr["key"] = converter.to_bytes(kd.Key_).c_str();

											root.append(KeyDescr);
										}
										key_queue.clear();

										Json::StreamWriterBuilder builder;
										string s_res = Json::writeString(builder, root);

										WCHAR_T* WCHART = nullptr;
										convToShortWchar(&WCHART, converter.from_bytes(s_res).c_str());

										m_CaddInNative->m_iConnect->ExternalEvent(s_EventSource, s_EventNameKeyHooked, WCHART);

										delete[] WCHART;
										WCHART = nullptr;

									}, ascii_code, wstring(unicode), hooked_key);           //в поток передаем объекты, а не указатели. В таком случае компилятор создаст код, 
                                                                                            //который скопирует эти объекты в стек запускаемого потока (будут созданы новые, независимые экземпляры объектов). 
                                t_0.detach();

							}
						}
					}
					else
					{
						if (hooked_key.scanCode == mOpenSymbol)
						{
							key_queue.push_back(KeyDescription(hooked_key.vkCode, hooked_key.scanCode, wstring(unicode)));
						}
					}

				}
				else
				{
					wchar_t lpszKeyName[1024];
					memset(lpszKeyName, 0, sizeof(wchar_t)*1024);

					SHIFT_key = GetAsyncKeyState(VK_SHIFT);
					ALT_key = GetAsyncKeyState(VK_MENU);
					CTRL_key = GetAsyncKeyState(VK_CONTROL);

					Json::Value root;

					root["Shift"] = SHIFT_key ? true : false;
					root["Ctrl"] = CTRL_key ? true : false;
					root["Alt"] = ALT_key ? true : false;

					wstring ws_key = wstring(tmp_wch);
					wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
					string s_key = converter.to_bytes(ws_key);


					stringstream stream_str;
					stream_str << std::hex << static_cast<int>(dwMsg);

					stringstream stream1_str;
					stream1_str << "0x";

					size_t l = 8 - stream_str.str().size();

					if (l > 0)
					{
						while (l)
						{
							stream1_str << "0";

							l--;
						}
						stream1_str << stream_str.rdbuf();
						root["KeyHex"] = stream1_str.str();
					}

					root["KeyStr"] = s_key.c_str();
                    root["sc"] = static_cast<int>(hooked_key.scanCode);
                    root["vc"] = static_cast<int>(hooked_key.vkCode);

					Json::StreamWriterBuilder builder;
					string s_res = Json::writeString(builder, root);

					std::wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;

					WCHAR_T* WCHART = nullptr;
					convToShortWchar(&WCHART, conv.from_bytes(s_res).c_str());

					m_CaddInNative->m_iConnect->ExternalEvent(s_EventSource, s_EventNameKeyHooked, WCHART);

					delete[] WCHART;
					WCHART = nullptr;

				}

			}

        }

    }
    
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

#pragma endregion

KeyDescription::KeyDescription(int in_vc, int in_sc, wstring in_key) : Vc(0), Sc(0)
{
    Vc = in_vc; 
    Sc = in_sc; 
    Key_ = in_key;
}

KeyDescription::~KeyDescription() {}
