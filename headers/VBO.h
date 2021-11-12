#pragma once
#include "debugging.h"

class VBO {
private:
    unsigned int ID;
public:
    VBO(const void* data, unsigned int size);
    ~VBO();
    void bind() const;
};
