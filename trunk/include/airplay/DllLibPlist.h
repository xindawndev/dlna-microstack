#ifndef _CXL_YETI_DLLLIBPLIST_H_
#define _CXL_YETI_DLLLIBPLIST_H_

#if defined(_WIN32)
#include <plist/plist.h>
#include <windows.h>
#else
typedef void * plist_t;
typedef unsigned int uint32_t;
#define NULL 0
#endif

class DllLibPlist
{
public:
    DllLibPlist() {}
    ~DllLibPlist() {
        if (is_loaded())
            unload();
    }
    bool load();

    bool is_loaded();

    void unload();

    void enable_delayed_unload(bool enabled);

    void plist_from_bin(const char *plist_bin, uint32_t length, plist_t * plist);

    bool plist_dict_get_size(plist_t node);

    plist_t plist_dict_get_item(plist_t node, const char* key);

    void plist_get_real_val(plist_t node, double *val);

    void plist_get_string_val(plist_t node, char **val);

    void plist_free_string_val(char *val);

    void plist_free(plist_t plist);

private:

#if defined(_WIN32)
    HINSTANCE m_dll_instance_;
#else
    int m_dll_instance_;
#endif
};

#endif // _CXL_YETI_DLLLIBPLIST_H_
