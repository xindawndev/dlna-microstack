#ifndef _CXL_YETI_IAIRPLAY_H_
#define _CXL_YETI_IAIRPLAY_H_

class IAirplay
{
public:
    virtual ~IAirplay() {}

    virtual int on_play() = 0;
    virtual int on_stop() = 0;
    virtual int on_pause() = 0;
    virtual int on_seek(__int64 pos) = 0;
    virtual int on_set_vol(double vol) = 0;
};

class MyAirplay : public IAirplay
{
public:
    MyAirplay() {}
    ~MyAirplay() {}

    virtual int on_play() {
        printf("\n++++++++++++++++++++++++++++++++\n");
        printf("\n+++++++++++++[On_Play]++++++++++\n");
        printf("\n++++++++++++++++++++++++++++++++\n");
        return 0;
    }
    virtual int on_stop() {
        printf("\n++++++++++++++++++++++++++++++++\n");
        printf("\n+++++++++++++[On_Stop]++++++++++\n");
        printf("\n++++++++++++++++++++++++++++++++\n");
        return 0;
    }
    virtual int on_pause() {
        printf("\n++++++++++++++++++++++++++++++++\n");
        printf("\n++++++++++++[On_Pause]++++++++++\n");
        printf("\n++++++++++++++++++++++++++++++++\n");
        return 0;
    }
    virtual int on_seek(__int64 pos) {
        printf("\n++++++++++++++++++++++++++++++++\n");
        printf("\n+++++++++++++[On_Seek]++++++++++\n");
        printf("\n++++++++++++++++++++++++++++++++\n");
        return 0;
    }
    virtual int on_set_vol(double vol) {
        printf("\n++++++++++++++++++++++++++++++++\n");
        printf("\n++++++++++[On_Set_Vol]++++++++++\n");
        printf("\n++++++++++++++++++++++++++++++++\n");
        return 0;
    }
};

#endif // _CXL_YETI_IAIRPLAY_H_
