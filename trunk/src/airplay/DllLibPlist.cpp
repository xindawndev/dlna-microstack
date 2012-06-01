#include "airplay/DllLibPlist.h"

bool DllLibPlist::load() 
{
    m_dll_instance_ = LoadLibraryA("plist.dll");
    if (m_dll_instance_) return true;
    return false;
}

bool DllLibPlist::is_loaded()
{
    return m_dll_instance_ != NULL;
}

void DllLibPlist::unload() 
{
    if (is_loaded()) {
        FreeLibrary(m_dll_instance_);
        m_dll_instance_ = NULL;
    }
}

void DllLibPlist::enable_delayed_unload(bool enabled)
{
}

void DllLibPlist::plist_from_bin(const char *plist_bin, uint32_t length, plist_t * plist) 
{
    if (!is_loaded()) return;
    typedef void (*Plist_from_bin)(const char *plist_bin, uint32_t length, plist_t * plist);
    Plist_from_bin inner_func   = NULL;
    inner_func                  =   (Plist_from_bin)GetProcAddress(m_dll_instance_, "plist_from_bin"); 
    if (inner_func) { 
        inner_func(plist_bin, length, plist); 
    } 
}

bool DllLibPlist::plist_dict_get_size(plist_t node)
{
    if (!is_loaded()) return false;
    typedef uint32_t (*Plist_dict_get_size)(plist_t node);
    Plist_dict_get_size inner_func   = NULL;
    inner_func                       =   (Plist_dict_get_size)GetProcAddress(m_dll_instance_, "plist_dict_get_size"); 
    if (inner_func) { 
        return inner_func(node) == 0; 
    } 
    return false;
}

plist_t DllLibPlist::plist_dict_get_item(plist_t node, const char* key)
{
    if (!is_loaded()) return (plist_t)NULL;
    typedef plist_t (*Plist_dict_get_item)(plist_t node, const char* key);
    Plist_dict_get_item inner_func   = NULL;
    inner_func                       =   (Plist_dict_get_item)GetProcAddress(m_dll_instance_, "plist_dict_get_item"); 
    if (inner_func) { 
        return inner_func(node, key); 
    } 
    return (plist_t)NULL;
}

void DllLibPlist::plist_get_real_val(plist_t node, double *val) 
{
    if (!is_loaded()) return;
    typedef void (*Plist_get_real_val)(plist_t node, double *val);
    Plist_get_real_val inner_func   = NULL;
    inner_func                       =   (Plist_get_real_val)GetProcAddress(m_dll_instance_, "plist_get_real_val"); 
    if (inner_func) { 
        return inner_func(node, val); 
    } 
}

void DllLibPlist::plist_get_string_val(plist_t node, char **val)
{
    if (!is_loaded()) return;
    typedef void (*Plist_get_string_val)(plist_t node, char**val);
    Plist_get_string_val inner_func   = NULL;
    inner_func                       =   (Plist_get_string_val)GetProcAddress(m_dll_instance_, "plist_get_string_val"); 
    if (inner_func) { 
        return inner_func(node, val); 
    } 
}

void DllLibPlist::plist_free_string_val(char *val)
{
    if (!is_loaded()) return;
    typedef void (*Plist_free_string_val)(char*val);
    Plist_free_string_val inner_func   = NULL;
    inner_func                         =   (Plist_free_string_val)GetProcAddress(m_dll_instance_, "plist_free_string_val"); 
    if (inner_func) { 
        return inner_func(val); 
    }
}

void DllLibPlist::plist_free(plist_t plist)
{
    if (!is_loaded()) return;
    typedef void (*Plist_free)(plist_t plist);
    Plist_free inner_func   = NULL;
    inner_func              =   (Plist_free)GetProcAddress(m_dll_instance_, "plist_free"); 
    if (inner_func) { 
        return inner_func(plist); 
    }
}
