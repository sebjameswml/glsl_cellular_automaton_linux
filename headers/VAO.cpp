#include "VAO.h"

VAO::VAO() {
    GLCall(glGenVertexArrays(1, &m_ID));
    GLCall(glBindVertexArray(m_ID));
}

VAO::~VAO() {
    GLCall(glDeleteVertexArrays(1, &m_ID));
}

void VAO::bind() const {
    GLCall(glBindVertexArray(m_ID));
}

void VAO::addBuffer(const VBO& vb, const VBO_layout& layout) {
    bind();
    vb.bind();
    const auto& elements = layout.getElements();
    unsigned long long int offset = 0;
    for (int i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.getStride(), (const void*)offset));
        offset += element.count * VBO_element::getSizeOfType(element.type);
    }
}
