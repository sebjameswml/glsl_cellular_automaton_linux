#pragma once
#include "debugging.h"

class IBO {
private:
    unsigned int m_ID;
    unsigned int m_Size;
public:
    IBO(const unsigned int* data, unsigned int count);
    ~IBO();
    void bind() const;
    inline unsigned int getSize() const { return m_Size; }
};
