//
// Created by ltc on 2021/3/6.
//

#include "ManageChunk.h"
#include <stdexcept>

ManageChunk::ManageChunk(size_t size, int num):size(size), num(num), head(nullptr), free(nullptr){}

void* ManageChunk::allocate(int num) {
    if (head == nullptr) {
        head = new MemChunk(num, size);
        return head->allocate();
    }
    MemChunk* curr = head;
    while (curr != nullptr){
        if (curr->num != 0){
            if (curr == free){
                free = nullptr;
            }
            if (curr == head){
                return head->allocate();
            }
            curr->prev->next = curr->next;
            if (curr->next != nullptr){
                curr->next->prev = curr->prev;
            }
            curr->prev = nullptr;
            curr->next = head;
            head->prev = curr;
            head = curr;
            return head->allocate();
        }
        curr = curr->next;
    }
    MemChunk* alloc = new MemChunk(num, size);
    alloc->next = head;
    head->prev = alloc;
    head = alloc;
    return head->allocate();
}

bool ManageChunk::deallocate(void* ptr, int num) {
    if (ptr != nullptr){
        MemChunk* curr = head;
        while (curr != nullptr){
            if (ptr >= (void*)(curr->start) && ptr < (void*)((char*)(curr->start)+size*num)){
                curr->deallocate(ptr);
                if (curr->num == num){
                    if (free != nullptr && free != curr){
                        if (free == head){
                            head = free->next;
                            head->prev = nullptr;
                        }else{
                            free->prev->next = free->next;
                            if (free->next != nullptr){
                                free->next->prev = free->prev;
                            }
                        }
                        delete free;
                    }
                    free = curr;
                }
                if (curr == head){
                    return true;
                }
                if (curr->prev != nullptr){
                    curr->prev->next = curr->next;
                }
                if (curr->next != nullptr){
                    curr->next->prev = curr->prev;
                }
                curr->prev = nullptr;
                curr->next = head;
                head->prev = curr;
                head = curr;
                return true;
            }
            curr = curr->next;
        }
    }
    return false;
}

ManageChunk::~ManageChunk() {
    int count = 0;
    while (head != nullptr){
        count += num - head->num;
        MemChunk* temp = head;
        head = head->next;
        delete temp;
    }
    if (count != 0) {
        std::cerr << "内存泄漏:" << count << "个" << size << "byte" << std::endl;
    }
}
