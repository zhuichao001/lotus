#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

template<class T>
class Node {
public:
    Node * next;
    T val;
};

template<class T>
class List {
    Node<T> *head, _;
    Node<T> *tail;
public:

    List() {
        head = tail = &_;
        head->next = nullptr;
    }

    ~List(){
        Node<T> *cur = head->next;
        while(cur){
            Node<T> *next = cur->next;
            delete cur;
            cur = next;
        }
    }

    bool empty(){
        return head->next == nullptr;
    }

    void push_back(T t) {
        Node<T> * e = new Node<T>();
        e->next = nullptr;
        e->val = t;

        Node<T> *last = nullptr;
        while (true) {
            last = this->tail;
            //if tailor is moved, try again
            if(last!=this->tail){
                continue;
            }

            Node<T> *back = last->next;
            //if tailor's next is not null, set tailor to back
            if(back!=nullptr){
                __sync_bool_compare_and_swap((uint64_t**)(&tail), (uint64_t*)last, (uint64_t*)back);
                continue;
            }

            if(__sync_bool_compare_and_swap((uint64_t**)(&tail->next), (uint64_t*)back, (uint64_t*)e)){
                break;
            }
        }
        __sync_bool_compare_and_swap((uint64_t**)(&tail), (uint64_t*)last, (uint64_t*)e);
    }

    bool pop_front(T *t) {
        Node<T> *first=nullptr;
        do {
            first = head->next;
            if (first == nullptr){
                return false;
            }
        } while (!__sync_bool_compare_and_swap((uint64_t**)(&head->next), (uint64_t*)first, (uint64_t*)(first->next)));

        first->next = nullptr;
        if (first==tail) {
            tail = head;
        }

        *t = first->val;
        delete first;
        return true;
    }
};
