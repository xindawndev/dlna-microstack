#ifndef _CXL_YETI_DLLLIBPLIST_H_
#define _CXL_YETI_DLLLIBPLIST_H_

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

    void enable_delayed_unload() {}
};

#endif // _CXL_YETI_DLLLIBPLIST_H_
