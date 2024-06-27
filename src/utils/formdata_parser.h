#ifndef FORMDATA_PARSER_H
#define FORMDATA_PARSER_H
#include<string>
#include<memory>
#include<vector>
using std::string;

class FormItem{
    friend class FormDataParser;
private:
    string m_fileName;
    string m_name;
    string m_contentType;
    const std::shared_ptr<string> m_content;
    int m_dataStart;
    int m_dataLength;
    FormItem(const string name,
        const string fileName,const string contentType,
        const std::shared_ptr<string> content,
        const int start,const int length);
public:
    inline string getFileName() const {return m_fileName;}
    inline string getName() const {return m_name;}
    inline string getContentType() const {return m_contentType;}
    inline bool isFile() const {return !m_fileName.empty();}
    //此处返回复制内容，看情况返回指针
    std::unique_ptr<string> getContent() const ;
};

class FormDataParser{
private:
    std::shared_ptr<string> m_data;
    string m_boundary;

    bool m_lastBoundaryFound;
    int m_pos;
    int m_lineStart;
    int m_lineLength;

    string m_iName;
    string m_iFileName;
    string m_iContentType;
    int m_iDataStart;
    int m_iDataLength;
public:
    FormDataParser(const std::shared_ptr<string> data,
        const int pos,const string boundary);
    std::unique_ptr<std::vector<FormItem>> parse();
private:
    void _parseHeaders();
    
};


#endif