#pragma once
#ifndef LIST_H
#define LIST_H

template <class T>
class list {
private:
  struct lNode {
    T data;
    lNode *next;
    lNode *prev;
  };
  lNode *head;
  lNode *tail;
  unsigned int count;
public:
  list();
  ~list();

  bool empty()        const { return count == 0; }
  unsigned int size() const { return count;      }
  void push_back(const T& data) {
    if(tail == 0) {
      tail = new lNode;
      tail->next = 0;
      tail->prev = 0;
      tail->data = data;
      head = tail;
      count++;
    }
    else {
      tail->next = new lNode;
      tail->next->prev = tail;
      tail = tail->next;
      tail->next = 0;
      tail->data = data;
      count++;
    }
  }

  class iterator {
  private:
    lNode *node;
  public:
    iterator(lNode *node) : node(node) {}
    bool operator==(iterator other) const { return node == other.node; }
    bool operator!=(iterator other) const { return node != other.node; }
    void operator++()    { if(node) node = node->next; }
    void operator++(int) { if(node) node = node->next; }
    T operator*()        { return node->data;          }
    friend class list<T>;
  };

  iterator begin() { return iterator(head); }
  iterator end()   { return iterator(0);    }
  iterator erase(iterator &it) {
    iterator tmp(0);
    lNode *node = it.node;

    if(node == 0)
      return tmp;

    if(node == head && node == tail) {
      head = 0;
      tail = 0;
      delete node;
      count--;
      return tmp;
    }

    if(node == head) {
      head = node->next;
      head->prev = 0;
      delete node;
      count--;
      tmp.node = head;
      return tmp;
    }

    if(node == tail) {
      tail = node->prev;
      tail->next = 0;
      delete node;
      count--;
      return tmp;
    }

    if(node->prev)
      node->prev->next = node->next;
    if(node->next)
      node->next->prev = node->prev;
    tmp.node = node->next;
    delete node;
    count--;

    return tmp; 
  }
};

template <class T>
list<T>::list() : head(0), tail(0), count(0) {}

template <class T>
list<T>::~list() {
  lNode *current = head;
  lNode *tmp;

  while(current != 0) {
    tmp = current;
    current = current->next;
    delete tmp;
  }
}

#endif /* LIST_H */

