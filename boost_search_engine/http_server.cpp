#include "cpp-httplib/httplib.h"
#include "searcher.hpp"

const std::string input = "data/raw_html/raw.txt";
const std::string root_path = "./wwwroot";

int main()
{
    NS_search::Searcher search;
    search.InitSearcher(input);

    httplib::Server svr;

    svr.set_base_dir(root_path.c_str());
    svr.Get("/s", [&search](const httplib::Request &req, httplib::Response &resp)
            {
                if (!req.has_param("word"))
                {
                    resp.set_content("请输入搜索关键字", "text/plain;charset=utf-8");
                    return;
                }
                std::string word = req.get_param_value("word");
                std::cout << "用户正在搜索：" << word << std::endl;
                std::string json_string;
                search.Search(word, &json_string);
                resp.set_content(json_string, "application/json");
                // resp.set_content("hello world!", "text/plain;charset=utf-8");
            });

    svr.listen("0.0.0.0", 8076);
    return 0;
}
