// 组合模式

/*
组合模式是一种结构性设计模式，旨在将对象组合成树形结构，以表示“部分-整体”的层次关系。
核心思想：把单个对象和组合对象用统一的方式对待，客户端无需区分他们是“叶子”还是“组合”。

优点：
1. 统一接口：叶子对象和组合对象可以用相同的方式处理单个对象和组合对象。
2. 简化代码：可以用相同的方式处理单个对象和组合对象；
3. 拓展性好：添加新的对象时，几乎不需要修改现有代码。

缺点：
1. 为了统一接口，可能要引入不必要的接口；
2. 单个对象和组合对象的区别被弱化，某些情况下，单个对象和组合对象的区别被强行统一，容易造成混淆。
*/

/*
案例：
设计一个简单的文件系统：
* 文件夹（Folder）可以包含文件（File）或子文件夹（Folder）；
* 每个元素都有 show() 方法来显示内容；
* 使用组合模式设计。
*/
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../util/util.h"

namespace dp::composite{
    class Composite{
    public:
        virtual void show(const std::string& preFile) = 0;
        virtual std::string get_name() = 0;
        virtual ~Composite() = default;
    };

    // 文件
    class File : public Composite{
    public:
        using file_t = std::string;
        File(const file_t& name, const file_t& content);
        ~File() = default;
        void show(const std::string& preFile) override;
        std::string get_name() override{
            return name_;
        }
    private:
        std::string name_;
        std::string content_;
    };

    class Folder : public Composite{
    public:
        using schild_t = std::shared_ptr<Composite>;

        explicit Folder(const std::string& name):name_(name), children_{}{}
        void show(const std::string& preFile) override;
        
        void add_child(schild_t child);
        void remove_child(schild_t child);

        std::string get_name() override{
            return name_;
        }

    private:
        std::string name_;
        std::vector<schild_t> children_; // 可能是文件也可能是文件夹
    };

    void test();
}