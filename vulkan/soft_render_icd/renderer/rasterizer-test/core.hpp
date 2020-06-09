#pragma once

#include <cstdint>
#include <cstddef>

#include <vector>

#include <glm/glm.hpp>

struct Viewport {
    size_t x, y, w, h;
    float n, f;
};

struct Framebuffer {
    Framebuffer(const Viewport& viewport): viewport{viewport} {
        colour.resize(viewport.w * viewport.h * 3);
        alpha.resize(viewport.w * viewport.h);
        depth.resize(viewport.w * viewport.h, viewport.f);
    }

    void write_pixel(size_t x, size_t y, glm::vec4 c){
        size_t offset = (y * viewport.w + x);
        colour[3 * offset] = c.r * 255;
        colour[3 * offset + 1] = c.g * 255;
        colour[3 * offset + 2] = c.b * 255;
        alpha[offset] = c.a * 255;
    }

    glm::vec4 read_pixel(size_t x, size_t y){
        size_t offset = (y * viewport.w + x);
        return glm::vec4{colour[3 * offset] / 255.0f, colour[3 * offset + 1] / 255.0f, colour[3 * offset + 2] / 255.0f, alpha[offset] / 255.0f};
    }

    Viewport viewport;
    std::vector<uint8_t> colour;
    std::vector<uint8_t> alpha; // TODO: Merge with colour
    std::vector<float> depth;
};
