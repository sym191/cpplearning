#pragma once

#include <utility>
#include <vector>


namespace ds{
template <typename T>
    class MinHeap{
    private:
        std::vector<T> heap_;

        inline std::size_t _father(std::size_t index){
            if(index == 0) return 0;
            return (index - 1) / 2;
        }

        inline std::size_t _lchild(std::size_t index){
            return (index * 2) + 1;
        }

        inline std::size_t _rchild(std::size_t index){
            return (index * 2) + 2;
        }

        void _up_node(std::size_t index){
            std::size_t size = heap_.size();
            if(index >= size) return;
            std::size_t father = _father(index);
            while(heap_[father] > heap_[index]){
                std::swap(heap_[father], heap_[index]);
                index = father;
                father = _father(index);
            }
        }
        void _down_node(std::size_t index){
            std::size_t size = heap_.size();
            if(index >= size) return;
            std::size_t lchild = _lchild(index), rchild = _rchild(index);
            while(lchild < size){
                if(rchild == size){
                    if(heap_[lchild] < heap_[index]){
                        std::swap(heap_[lchild], heap_[index]);
                        index = lchild;
                    }
                    else{
                        break;
                    }
                }
                else{
                    if(heap_[lchild] < heap_[rchild]){
                        if(heap_[lchild] < heap_[index]){
                            std::swap(heap_[lchild], heap_[index]);
                            index = lchild;
                        }
                        else{
                            break;
                        }
                    }
                    else{
                        if(heap_[rchild] < heap_[index]){
                            std::swap(heap_[rchild], heap_[index]);
                            index = rchild;
                        }
                        else{
                            break;
                        }
                    }
                }
                lchild = _lchild(index), rchild = _rchild(index);
            }
        }
    public:
        T& top() {
            return heap_[0];
        }
        void pop(){
            auto size = heap_.size();
            if(size <= 0) return;
            std::swap(heap_[0], heap_[size-1]);
            heap_.pop_back();
            _down_node(0);
        }
        template <typename U>
        void push(U&& val){
            heap_.push_back(std::forward<U>(val));
            _up_node(heap_.size() - 1);
        }
        [[nodiscard]] std::size_t size() const {
            return heap_.size();
        }
        [[nodiscard]] bool empty() const {
            return heap_.empty();
        }
    };
}
