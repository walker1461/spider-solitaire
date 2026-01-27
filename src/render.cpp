#include "render.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <filesystem>

static float cardVertices[] = {
    // positions        // texCoords
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f,

    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
};

GLuint loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    if (unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0)) {
        const GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture: " << path << "\n";
        stbi_image_free(data);
    }
    return textureID;
};

unsigned int createCardVAO() {
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cardVertices), cardVertices, GL_STATIC_DRAW);

    // tell OpenGL the vertex layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
};

unsigned int createShaderProgram() {
    const auto vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        uniform vec2 offset;
        uniform vec2 uSize;
        uniform float uScale;

        out vec2 TexCoords;
        out vec2 localPos;

        void main() {
            localPos = aPos.xy * uSize * uScale;
            gl_Position = vec4(localPos + offset, aPos.z, 1.0);
            TexCoords = aTexCoord;
        }
    )";

    const auto fragmentShaderSource = R"(
        #version 330 core
        in vec2 localPos;
        in vec2 TexCoords;
        out vec4 FragColor;

        uniform sampler2D cardTexture;
        uniform vec3 uColor;
        uniform float radius;
        uniform vec2 uSize;
        uniform float uScale;

        void main() {
            vec2 halfSize = 0.5 * uSize * uScale;
            float r = radius * uScale;

            vec2 p = abs(localPos);
            vec2 q = p - halfSize + vec2(r);
            float dist = length(max(q, 0.0));

            if (dist > r) {
                discard;
            }
            FragColor = texture(cardTexture, TexCoords);
        }
    )";

    auto compileShader = [](unsigned int type, const char* src) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    };

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
};

void renderCard(const Card& card, const unsigned int shaderProgram, const unsigned int VAO, const unsigned int cardBack) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glUniform2f(glGetUniformLocation(shaderProgram, "offset"),
                card.position.x, card.position.y);

    glUniform2f(glGetUniformLocation(shaderProgram, "uSize"),
                card.size.x, card.size.y);

    glUniform1f(glGetUniformLocation(shaderProgram, "uScale"), card.scale);
    glUniform1f(glGetUniformLocation(shaderProgram, "radius"), card.cornerRadius);

    glActiveTexture(GL_TEXTURE0);
    if (card.faceUp) {
        glBindTexture(GL_TEXTURE_2D, card.textureID);
    } else {
        glBindTexture(GL_TEXTURE_2D, cardBack);
    }
    glUniform1i(glGetUniformLocation(shaderProgram, "cardTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
};