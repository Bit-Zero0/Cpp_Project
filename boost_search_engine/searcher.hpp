#pragma once

#include <jsoncpp/json/json.h>
#include "index.hpp"
#include "log.hpp"

namespace NS_search
{

    struct InvertedElemPrint
    {
        uint64_t doc_id;
        int weight;
        std::vector<std::string> words;

        InvertedElemPrint() : doc_id(0), weight(0) {}
    };

    class Searcher
    {
    private:
        NS_index::Index *index;

    public:
        Searcher(){};
        ~Searcher(){};

        void InitSearcher(const std::string &input)
        {
            // 获取或者创建index对象
            index = NS_index::Index::GetInstance();
			LOG(NORMAL, "获取index单例成功...");

			// 根据index对象建立索引
            index->BuildIndex(input);
			LOG(NORMAL, "建立正排和倒排索引成功...");
        }

        void Search(const std::string &query, std::string *json_string)
        {
            // 分词:对我们的query进行按照searcher的要求进行分词
            std::vector<std::string> words;
            NS_util::JiebaUtil::CutString(query, &words);

            // 触发:就是根据分词的各个"词"，进行ind ex查找,建立index是忽略大小写，所以搜索，关键字也需要
            std::vector<InvertedElemPrint> inverted_list_all;
            std::unordered_map<uint64_t, InvertedElemPrint> tokens_map;
            for (auto word : words)
            {
                boost::to_lower(word);

                NS_index::InvertedList *inverted_list = index->GetInveredList(word);
                if (nullptr == inverted_list)
                {
                    continue;
                }

                for (const auto &elem : *inverted_list)
                {
                    // item一定是doc_id相同的print节点
                    auto &item = tokens_map[elem.doc_id];
                    item.doc_id = elem.doc_id;
                    item.weight += elem.weight;
                    item.words.push_back(elem.word);
                }
            }
            for (const auto &item : tokens_map)
            {
                inverted_list_all.push_back(std::move(item.second));
            }

            // 合并排序：汇总查找结果，按照相关性(weight)降序排序
            std::sort(
                inverted_list_all.begin(), inverted_list_all.end(), [](const InvertedElemPrint &e1, const InvertedElemPrint &e2)
                { return e1.weight > e2.weight; });

            Json::Value root;
            for (auto &item : inverted_list_all)
            {
                NS_index::DocInfo *doc = index->GetForwardIndex(item.doc_id);
                if (nullptr == doc)
                {
                    continue;
                }
                Json::Value elem;
                elem["title"] = doc->title;
                elem["desc"] = GetDesc(doc->content, item.words[0]);
                elem["url"] = doc->url;
                elem["id"] = (int)item.doc_id;
                elem["weight"] = item.weight;

                root.append(elem);
            }

            // Json::FastWriter writer;
            Json::StyledWriter writer;
            *json_string = writer.write(root);
        }

        // 找到word在html_content中的首次出现，然后往前找50字节(如果没有，从begin开始)，往后找100字节(如果没有，到end就可以的)
        // 截取出这部分内容
        std::string GetDesc(const std::string &html_content, const std::string word)
        {
            const int prev_step = 50;
            const int next_step = 50;
            // 1. 找到首次出现的关键词，为什么不使用find，因为find是区分大小写的
            auto iter = std::search(html_content.begin(), html_content.end(), word.begin(), word.end(), [](int x, int y)
                                    { return (std::tolower(x) == std::tolower(y)); });
            if (iter == html_content.end())
            {
                return "None!";
            }

            int pos = std::distance(html_content.begin(), iter);

            int start = 0;
            int end = html_content.size() - 1;

            // 如果之前有50+字符，就更新开始位置
            if (pos > start + prev_step)
                start = pos - prev_step;
            if (pos < end - next_step)
                end = pos + next_step;

            if (start >= end)
                return "None2";
            std::string desc = html_content.substr(start, end - start);
            desc += "...";
            return desc;
        }
    };
}