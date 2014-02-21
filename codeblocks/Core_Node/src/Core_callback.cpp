#include "../include/Core_callback.h"

#if 0
Params CB_MessageDeal::MURegister(Params in_params)
{
    std::cout<<"This is MURegister callbakc"<<std::endl;
    Params inp = in_params;
    Params::iterator iter;
    for(iter=inp.begin(); iter != inp.end(); iter++) {
        std::cout<<(*iter).first<<" : "<<(*iter).second<<std::endl;
    }
    Params outp;
    outp["code"] = "0";
    outp["result"] = "success";
    outp["muKeepAlivePeriod"] = "100s";

    return outp;
}
#endif

#if 0
    std::map<std::string, FuncT> Core_callback::mapFunc;

    Core_callback::Core_callback() {
        //ctor
        if (mapFunc.empty()) {//初始化
            mapFunc["one"] = &Core_callback::MURegister;


            mapFunc["two"] = &Core_callback::onFunc2;
            mapFunc["three"] = &Core_callback::onFunc3;
            mapFunc["four"] = &Core_callback::onFunc4;
        }
    }

    Core_callback::~Core_callback() {
        //dtor
    }


//根据key 和 参数in_param 调用对应的函数
    void Core_callback::dispatch(string key, Params in_param) {
        if (mapFunc.find(key) != mapFunc.end()) {
            (this->*mapFunc[key])(in_param);
        }
    }
#if 0

    Params Core_callback::onFunc1(Params param) {
        Params out = param;
        out["ten"] = "hello world";
    }

    Params Core_callback::onFunc2(Params param) {
    }

    Params Core_callback::onFunc3(Params param) {
    }

    Params Core_callback::onFunc4(Params param) {
    }
#endif

#endif
