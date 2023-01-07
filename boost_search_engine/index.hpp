#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <mutex>

#include "Util.hpp"
#include "log.hpp"

namespace NS_index
{

    struct DocInfo
    {
        std::string title;
        std::string content;
        std::string url;
        uint64_t doc_id; // 文档id
    };

    struct InvertedElem
    {
        uint64_t doc_id;
        std::string word;
        int weight;
    };

    typedef std::vector<InvertedElem> InvertedList; // 倒排拉链

    class Index
    {
    private:
        // 正排索引的数据结构用数组，数组的下标天然是文档的ID
        std::vector<DocInfo> forward_index;

        // 倒排索引一定是一个关键字和一组(个)InvertedElem对应[关键字和倒排拉链的映射关系]
        std::unordered_map<std::string, InvertedList> inverted_index;

    private:
        Index(){};
        Index(const Index &) = delete;
        Index &operator=(const Index &) = delete;

        static Index *instance;
        static std::mutex mtx;

    public:
        ~Index(){};

        static Index *GetInstance()
        {
            if (nullptr == instance)
            {
                mtx.lock();
                if (nullptr == instance)
                {
                    instance = new Index();
                }
                mtx.unlock()
            }

            return instance;
        }

        // 根据doc_id找到找到文档内容
        DocInfo *GetForwardIndex(uint64_t doc_id)
        {
            if (doc_id >= forward_index.size())
            {
                std::cerr << "doc_id out range ,error" << std::endl;
                return nullptr;
            }
            return &forward_index[doc_id];
        }

        // 根据关键字string，获得倒排拉链
        InvertedList *GetInveredList(const std::string &word)
        {
            auto iter = inverted_index.find(word);
            if (iter == inverted_index.end())
            {
                std::cerr << word << "have no InvertedList" << std::endl;
                return nullptr;
            }
            return &(iter->second);
        }

        // 根据去标签，格式化之后的文档，构建正排和倒排索引
        bool BuildIndex(const std::string &input)
        {
            std::ifstream in(input, std::ios::in | std::ios::binary);
            if (!in.is_open())
            {
                std::cerr << "sorry ," << input << "open error " << std::endl;
                return false;
            }

            std::string line;
            int count = 0;
            while (std::getline(in, line))
            {
                DocInfo *doc = BulidForwardIndex(line);
                if (nullptr == doc)
                {
                    std::cerr << "build" << line << "error" << std::endl;
                    continue;
                }

                BuildInvertedIndex(*doc);
				count++;
                LOG(NORMAL, "当前已经建立的索引文档" + std::to_string(count));
            }
            return true;
        }

    private:
        DocInfo *BulidForwardIndex(const std::string &line)
        {
            // 解析line，字符串切分
            std::vector<std::string> results;
            const std::string sep = "\3";
            NS_util::StringUtil::Split(line, &results, sep);
            if (results.size() != 3)
            {
                return nullptr;
            }

            // 字符串进行填充到DocIinfo
            DocInfo doc;
            doc.title = results[0];
            doc.content = results[1];
            doc.url = results[2];
            doc.doc_id = forward_index.size();
            forward_index.push_back(std::move(doc)); // 先进行保存id，在插入，对应的id就是当前doc在vector中的下标!

            // 插入到正排索引的vector
            return &forward_index.back();
        }

        bool BuildInvertedIndex(const DocInfo &doc)
        {
            // DocInfo{title, content, url, doc_id}
            struct word_cnt
            {
                int title_cnt;
                int content_cnt;

                word_cnt() : title_cnt(0), content_cnt(0){};
            };

            std::unordered_map<std::string, word_cnt> word_map; // 暂存词频的映射表

            // 对标题进行分词
            std::vector<std::string> title_words;
            NS_util::JiebaUtil::CutString(doc.title, &title_words);

            // 对标题进行词频统计
            for (std::string s : title_words)
            {
                boost::to_lower(s); ////需要统一转化成为小写,因为一般搜索引擎不区分大小写
                word_map[s].title_cnt++;
            }

            // 对文档内容进行分词
            std::vector<std::string> content_words;
            NS_util::JiebaUtil::CutString(doc.content, &content_words);

            // 对内容进行词频统计
            for (std::string s : content_words)
            {
                boost::to_lower(s);
                word_map[s].content_cnt++;
            }

#define X 10
#define Y 1
            for (auto &word_pair : word_map)
            {
                InvertedElem item;
                item.doc_id = doc.doc_id;
                item.weight = word_pair.second.title_cnt * X + word_pair.second.content_cnt * Y;
                item.word = word_pair.first;
                InvertedList &inverted_list = inverted_index[word_pair.first];
                inverted_list.push_back(std::move(item));
            }

            return true;
        }
    };
    Index *Index::instance = nullptr;
    std::mutex Index::mtx;
}
