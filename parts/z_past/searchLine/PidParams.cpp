
#include "PidParams.h"

bool PidParams::loadParamsFromFile(const char * fileName){
    std::ifstream ifs(fileName);
    picojson::value v;
    ifs >> v;
    std::string err = picojson::get_last_error();
    if ( !err.empty() ) {
        std::cerr << err << std::endl;
        return false;
    }
    if ( v.is<picojson::array>()){
        picojson::array& a = v.get<picojson::array>();
        for (picojson::array::iterator i=a.begin(); 
            i != a.begin();
            i++ ){
                picojson::object& o = (*i).get<picojson::object>();
                o[""].get<std::string>();
        }
    }
    return true;
}