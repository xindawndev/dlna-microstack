#ifndef _CXL_YETI_DLLLIBPLIST_H_
#define _CXL_YETI_DLLLIBPLIST_H_

#include <plist/plist.h>

class DllLibPlist
{
public:
    DllLibPlist() {}
    bool load() { return true; }

    bool is_loaded() {
        return true;
    }

    void unload() {

    }

    void enable_delayed_unload(bool enabled) {}

    void plist_from_bin(const char *plist_bin, uint32_t length, plist_t * plist) {}

    bool plist_dict_get_size(plist_t node ) {
        return true;
    }

    plist_t plist_dict_get_item(plist_t node, const char* key ) { return (plist_t)NULL; }

    void plist_get_real_val(plist_t node, double *val) {}

    void plist_get_string_val(plist_t node, char **val) {}

    void plist_free_string_val(char *val) {}

    void plist_free(plist_t plist ) {}
};

#endif // _CXL_YETI_DLLLIBPLIST_H_
