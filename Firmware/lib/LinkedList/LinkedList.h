#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include "Arduino.h"

template <typename T> class Node {
public:
  T value;
  Node<T> *next;

  Node(T data) : value(data), next(NULL) {}
};

template <typename T> class LinkedList {
private:
  Node<T> *mRoot;

public:
  void push_back(T data) {
    if (this->mRoot == NULL) {
      this->mRoot = new Node<T>(data);
      return;
    }

    Node<T> *temp = this->mRoot;

    while (temp->next != 0) {
      temp = temp->next;
    }

    temp->next = new Node<T>(data);
  }

  T pop() {
    Node<T> *temp = this->mRoot;

    while (temp->next != NULL && temp->next->next != NULL) {
      temp = temp->next;
    }

    T data = temp->next->value;
    delete temp->next;
    temp->next = NULL;
    return data;
  }

  int size() {
    int counter = 0;
    Node<T> *temp = mRoot;
    while (temp->next != 0) {
      temp = temp->next;
      counter++;
    }
    return counter;
  }

  T at(int index) {
    Node<T> *temp = this->mRoot;
    if (temp == NULL) {
      return NULL;
    }
    while (index-- > 0) {
      if (temp->next != NULL) {
        temp = temp->next;
      } else {
        return NULL;
      }
    }
    return temp->value;
  }

  void clear() {
    int size = this->size();
    while (size-- > 0) {
      delete this->pop();
    }
  }

  void del(T data) {
    Node<T> *temp = this->mRoot;
    if (temp == NULL) {
      return NULL;
    }

    Node<T> *prev = NULL;

    while (temp->next != NULL) {
      if (temp->data == data) {
        if (prev == NULL) {
          Node<T> *next = temp->next;
          delete mRoot;
          mRoot = next;
        } else {
          prev->next = temp->next;
          delete temp;
        }
        return;
      }
      prev = temp;
      temp = temp->next;
    }
  }
};
#endif // _LINKEDLIST_H_
