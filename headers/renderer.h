#pragma once

#include "shaderClass.h"
#include "IBO.h"
#include "VAO.h"
#include "debugging.h"

#include <GLFW/glfw3.h>

class renderer {
public:
    void draw(const VAO& vao, const IBO& ibo, const shaderClass& shader) const;
};
