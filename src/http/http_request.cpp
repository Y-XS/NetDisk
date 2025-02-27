#include"http_request.h"

HttpRequest::HttpRequest(){
    m_state = PARSE_STATE::START;
    m_nextPos = 0;
}
void HttpRequest::parse(std::string requestStr){
    int len = requestStr.size();
    
    while(m_state!=PARSE_STATE::INVALID
            &&m_state!=PARSE_STATE::INVALID_HEADER
            &&m_state!=PARSE_STATE::INVALID_METHOD
            &&m_state!=PARSE_STATE::INVALID_URI
            &&m_state!=PARSE_STATE::INVALID_VERSION
            &&m_state!=PARSE_STATE::COMPLEIE
    ){
        switch(m_state){
            case PARSE_STATE::START:{
                LOG_DEBUG("=====START======");
                // std::cout<<requestStr<<std::endl;
                m_state = PARSE_STATE::REQUESTLINE;
                break;
            }
            case PARSE_STATE::REQUESTLINE:{
                LOG_DEBUG("=====REQUESTLINE======");
                int reqLineLen = requestStr.find("\r\n");
                std::string requestLine = requestStr.substr(0,reqLineLen);
                int pos = requestLine.find(" ");
                int rpos = requestLine.rfind(" ");
                m_method = str2method(requestLine.substr(0,pos));
                m_url = requestLine.substr(pos+1,rpos-pos-1);
                m_version = requestLine.substr(rpos+1,len);
                m_nextPos += reqLineLen;
                m_state = PARSE_STATE::HEADER;
                // cout<<method2str(m_method)<<" "<<m_url<<endl;
            }
            case PARSE_STATE::HEADER:{
                LOG_DEBUG("=====HEADER======");
                //正则表达式要求gcc版本
                // regex patten("^([^:]*): ?(.*?)$");
                // smatch subMatch;
                // if(regex_match(requestStr, subMatch, patten)){
                //     m_params[subMatch[1]] = subMatch[2];
                // }
                int start = requestStr.find("Content-Length: ") + strlen("Content-Length: ");
                int end = requestStr.find("\r\n", start);
                m_bodySize = atoi(requestStr.substr(start,end-start).c_str());

                std::string tmp;
                int startPos = requestStr.find("\r\n") + strlen("\r\n");
                do{
                    int pos = requestStr.find("\r\n",startPos);
                    tmp = requestStr.substr(startPos, pos - startPos);
                    if(tmp.find(":") != std::string::npos){
                        std::string key = tmp.substr(0, tmp.find(":"));
                        std::string value = tmp.substr(tmp.find(":")+1,tmp.size()-tmp.find(":"));
                        m_headers[key] = value;
                    }
                    startPos = pos + strlen("\r\n");
                }while(startPos < requestStr.size() - m_bodySize && startPos > 0);
                m_state = PARSE_STATE::BODY;
            }
            case PARSE_STATE::BODY:{
                cout<<"=====BODY======"<<endl;
                // LOG_DEBUG("=====BODY======");
                if(m_bodySize == 0){
                    m_state = PARSE_STATE::COMPLEIE;
                    break;
                }
                m_body = requestStr.substr(requestStr.size() - m_bodySize, m_bodySize);
                // cout<<"------m_body-----\n"<<m_body<<endl;
                std::string tmp = m_body;
                // cout<<"------tmp-----"<<tmp<<endl;

                std::vector<std::string> vec;
                int pos = 0;
                while((pos = tmp.find("&"))!=std::string::npos){
                    vec.push_back(tmp.substr(0,pos));
                    tmp = tmp.substr(pos+1,m_body.size());
                }
                vec.push_back(tmp);
                for(std::string item:vec){
                    int pos = item.find("=");
                    std::string key = item.substr(0,pos);
                    std::string value = item.substr(pos+1,item.size());
                    // cout<<key<<" "<<value<<endl;
                    m_post_params[key] = value;
                }
                // cout<<"m_post_params"<<endl;
                // for(auto it:m_post_params){
                //     cout<<it.first<<" "<<it.second<<endl;
                // }

                //神奇BUG
                // int left = 0, right = 0,end = 0, flag=0;
                // while(tmp.size()>0 && flag==0){
                //     cout<<"-----------"<<endl;
                //     right = tmp.find("=");
                //     std::string key = tmp.substr(left,right);

                //     end = tmp.find("&");
                //     cout<<"end="<<end<<endl;
                //     if(end==std::string::npos){
                //         end = tmp.size();
                //         flag++;
                //     }
                //     cout<<"end="<<end<<endl;
                //     cout<<"right="<<right<<endl;
                //     std::string value = tmp.substr(right+1,end);
                //     std::string val = tmp.substr(8+1,14);
                //     cout<<"------tmp-----"<<tmp<<endl;
                //     tmp = m_body.substr(end+1,m_body.size());
                //     cout<<key<<" "<<value<<" "<<val<<" "<<tmp<<endl;
                // }
                m_state = PARSE_STATE::COMPLEIE;
            }
        }
    }
    m_state = PARSE_STATE::START;
    // cout<<"***** parse complete *****"<<endl;
    // for(auto it = m_headers.begin();it!=m_headers.end();it++){
    //     cout<<it->first<<" "<<it->second<<endl;
    // }
}

std::string HttpRequest::method2str(METHOD method)
{
    switch(method){
        case METHOD::GET:return "GET";
        case METHOD::POST:return "POST";
        case METHOD::PUT:return "PUT";
        case METHOD::DELETE:return "DELETE";
    }
}

METHOD HttpRequest::str2method(std::string str)
{
    //暂时支持4种
    if(0 == str.compare("GET")) return METHOD::GET;
    if(0 == str.compare("POST")) return METHOD::POST;
    if(0 == str.compare("PUT")) return METHOD::PUT;
    if(0 == str.compare("DELETE")) return METHOD::DELETE;
}

HttpRequest::~HttpRequest(){
}
