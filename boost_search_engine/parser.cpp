#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

const std::string src_path = "data/input";
const std::string output = "data/raw_html/raw.txt";

typedef struct DocInfo{
    std::string title;
    std::string content;
    std::string url;
}DocInfo_t;


bool EnumFile(const std::string& src_path , std::vector<std::string>* files_list );
bool parseHtml(const std::vector<std::string>& files_list  , std::vector<DocInfo_t>* results);
bool saveHtml(const std::vector<DocInfo_t>& results ,const std::string& output);

int main()
{
    std::vector<std::string> files_list;
    if(!EnumFile(src_path , &files_list))
    {
        std::cout << "enum file name error"<< std::endl;
        return 1;
    }

    std::vector<DocInfo_t> results;
    if(!parseHtml(files_list , &results))
    {
        std::cout << "parse html error" << std::endl;
        return 2;
    }

    if(!saveHtml(results , output))
    {
        std::cout << "save html error" << std::endl;
        return 3;
    }

    return 0;
}



bool EnumFile(const std::string& src_path , std::vector<std::string>* files_list )
{
    namespace fs = boost::filesystem;
    fs::path root_path(src_path);

    if(!fs::exists(root_path))
    {
        std::cerr<< src_path << "not exists" << std::endl;
        return false;
    }

    fs::recursive_directory_iterator end;
    for(fs::recursive_directory_iterator iter(root_path); iter != end ; iter++)
    {
        //判断文件是否是普通文件，html都是普通文件
        if(!fs::is_regular_file(*iter))
        {
            continue;
        }

        //判断文件路径名的后缀是否符合要求
        if(iter->path().extension() != ".html")
        {
            continue;
        }

        //std::cout << "debug:" << iter->path().string() << std::endl;
        //当前的路径一定是一个合法的，以.html结束的普通网页文件
        files_list->push_back(iter->path().string());//将所有带路径的html保存在files_list,方便后续进行文本分析
    }
    
    return true;
}



bool parseHtml(const std::vector<std::string>& files_list  , std::vector<DocInfo_t>* results)
{
    return true;
}



bool saveHtml(const std::vector<DocInfo_t>& results ,const std::string& output)
{
    return true;
}
