#include "composite.h"
#include <algorithm>
#include <memory>


namespace dp::composite{
    File::File(const file_t& name, const file_t& content):
    name_(name), content_(content)
    {}

    void File::show(const std::string& preFile){
        log("{}/{}: {}", preFile, name_, content_);
    }

    void Folder::show(const std::string& preFile){
        auto pre = preFile + "/" + name_;
        log("{}/{}/", preFile, name_);
        for(auto& child : children_){
            child -> show(pre);
        }
    }

    void Folder::add_child(schild_t child){
        auto pos = std::find_if(children_.begin(), children_.end(), [&, this](schild_t& c){
            // warn("cur node: {}; new node: {}", c->get_name(), child->get_name());
            return c->get_name() == child->get_name();
        });
        if(pos != children_.end()){
            log("已经存在相同的文件，请修改名字后重试"); // 避免处理化，禁止文件夹和文件同名;
            return;
        }
        children_.push_back(std::move(child));
    }

    void Folder::remove_child(schild_t child){
        auto pos = std::find_if(children_.begin(), children_.end(), [&, this](schild_t& c){
            return c->get_name() == child->get_name();
        });
        if(pos == children_.end()){
            log("文件不存在"); // 避免处理化，禁止文件夹和文件同名;
            return;
        }
        children_.erase(pos);
    }

    void test(){
        Folder root{"root"};
        root.show(" ");
        auto f1 = std::make_shared<Folder>("sub_folder");
        auto f2 = std::make_shared<Folder>("sub_folder2");
        auto file = std::make_shared<File>("File", "hello world!");
        log("-------------------------------");
        root.add_child(f1); root.add_child(f2);
        root.show(" ");
        log("-------------------------------");
        root.add_child(f1);root.add_child(f2);root.add_child(file);root.show(" ");
        log("-------------------------------");
        f1->add_child(file);
        log("-------------------------------");
        root.show(" ");
    }
}