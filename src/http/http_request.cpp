#include"http_request.h"

HttpRequest::HttpRequest(){
    m_state = PARSE_STATE::START;
}
void HttpRequest::parse(std::string requestStr){
    // std::string str;
    // for(int i=0;i<len;i++){
    //     std::cout<<buf[i];
    // }
    int len = requestStr.size();
    
    // char* p0 = const_cast<char*>(buf);
    while(m_state!=PARSE_STATE::INVALID
            &&m_state!=PARSE_STATE::INVALID_HEADER
            &&m_state!=PARSE_STATE::INVALID_METHOD
            &&m_state!=PARSE_STATE::INVALID_URI
            &&m_state!=PARSE_STATE::INVALID_VERSION
            &&m_state!=PARSE_STATE::COMPLEIE
        ){
            // char ch = *p0;
            // char* p = p0++;
            // int pos = m_nextPos++;
            switch(m_state){
                case PARSE_STATE::START:{
                    int pos = requestStr.find("\r\n");
                    std::cout<<len<<" "<<pos<<std::endl;
                    std::string requestLine = requestStr.substr(0,pos);
                    std::cout<<requestLine<<std::endl;
                    break;
                    // if(isupper())
                }
            }
        break;
    }
}

HttpRequest::~HttpRequest(){

}
