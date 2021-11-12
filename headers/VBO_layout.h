#pragma once
#include <string>
#include <vector>
#include "VBO.h"

struct VBO_element {
    unsigned int type;
    unsigned int count;
    unsigned char normalised;

    static unsigned int getSizeOfType(unsigned int type) {
        switch (type) {
        case GL_FLOAT:          return 4;
        case GL_UNSIGNED_INT:   return 4;
        case GL_UNSIGNED_BYTE:  return 1;
        }
    }
};

class VBO_layout{
private:
    std::vector<VBO_element> m_elements;
    unsigned int m_stride;
public:
    VBO_layout() : m_stride(0) {};

    void pushFloat(unsigned int count);
    void pushInt(unsigned int count);
    void pushChar(unsigned int count);

    const std::vector<VBO_element> getElements() const;
    unsigned int getStride() const;
};
