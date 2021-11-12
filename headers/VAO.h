#pragma once
#include "VBO.h"
#include "VBO_layout.h"
#include "debugging.h"

class VAO {
private:
    unsigned int m_ID;
public:
    VAO();
    ~VAO();
    void addBuffer(const VBO& vb, const VBO_layout& layout);
    void bind() const;
};
