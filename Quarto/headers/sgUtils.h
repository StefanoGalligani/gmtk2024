#pragma once

#include <sgModel.h>
#include <GL/glew.h>
#include <chrono>
#include <fstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace sg {

    GLuint CompileShader(const char* source, GLuint shaderType) {
        GLuint shaderID;
        std::ifstream shaderStream(source, std::ios::in);
        if (!shaderStream.is_open()) {
            std::cout << "ERROR: Cannot open file." << std::endl;
            return false;
        }

        std::string shaderSourceCode((std::istreambuf_iterator<char>(shaderStream)), std::istreambuf_iterator<char>());
        shaderStream.close();

        shaderID = glCreateShader(shaderType);
        char const *shaderData = shaderSourceCode.data();
        glShaderSource(shaderID, 1, &shaderData, nullptr);
        glCompileShader(shaderID);

        GLint result = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

        int infoLogLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 1) {
            std::vector<char> compilerMessage(infoLogLength);
            glGetShaderInfoLog(shaderID, infoLogLength, nullptr, compilerMessage.data());
            if (!result) {
                std::cout << "ERROR: Cannot compile shader " << source << " ; " << compilerMessage.data() << std::endl;
                glDeleteShader(shaderID);
                return false;
            }
        }

        if (result) {
            GLint stype;
            glGetShaderiv(shaderID, GL_SHADER_TYPE, &stype);
            if (stype != (GLint)shaderType) {
                std::cout << "ERROR: Incorrect shader type." << std::endl;
                return false;
            }
        }
        return shaderID;
    }

    GLuint CreateProgram(const char* vsSource, const char* fsSource) {
        GLuint vsID = CompileShader(vsSource, GL_VERTEX_SHADER);
        GLuint fsID = CompileShader(fsSource, GL_FRAGMENT_SHADER);

        GLuint programID = glCreateProgram();
        glAttachShader(programID, vsID);
        glAttachShader(programID, fsID);

        glLinkProgram(programID);
        GLint result = GL_FALSE;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        int infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 1) {
            std::vector<char> compilerMessage(infoLogLength);
            glGetProgramInfoLog(programID, infoLogLength, nullptr, compilerMessage.data());
            std::cout << "ERROR: " << compilerMessage.data() << std::endl;
        }

        glUseProgram(programID);

        glDeleteShader(vsID);
        glDeleteShader(fsID);

        return programID;
    }

    GLuint CreateProgram(const char* vsSource, const char* fsSource, const char* gsSource) {
        GLuint vsID = CompileShader(vsSource, GL_VERTEX_SHADER);
        GLuint fsID = CompileShader(fsSource, GL_FRAGMENT_SHADER);
        GLuint gsID = CompileShader(gsSource, GL_GEOMETRY_SHADER);

        GLuint programID = glCreateProgram();
        glAttachShader(programID, vsID);
        glAttachShader(programID, fsID);
        glAttachShader(programID, gsID);

        glLinkProgram(programID);
        GLint result = GL_FALSE;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        int infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 1) {
            std::vector<char> compilerMessage(infoLogLength);
            glGetProgramInfoLog(programID, infoLogLength, nullptr, compilerMessage.data());
            std::cout << "ERROR: " << compilerMessage.data() << std::endl;
        }

        glUseProgram(programID);

        glDeleteShader(vsID);
        glDeleteShader(fsID);
        glDeleteShader(gsID);

        return programID;
    }

    GLuint CreateProgram(const char* vsSource, const char* fsSource, const char* tcsSource, const char* tesSource) {
        GLuint vsID = CompileShader(vsSource, GL_VERTEX_SHADER);
        GLuint fsID = CompileShader(fsSource, GL_FRAGMENT_SHADER);
        GLuint tcsID = CompileShader(tcsSource, GL_TESS_CONTROL_SHADER);
        GLuint tesID = CompileShader(tesSource, GL_TESS_EVALUATION_SHADER);

        GLuint programID = glCreateProgram();
        glAttachShader(programID, vsID);
        glAttachShader(programID, fsID);
        glAttachShader(programID, tcsID);
        glAttachShader(programID, tesID);

        glLinkProgram(programID);
        GLint result = GL_FALSE;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        int infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 1) {
            std::vector<char> compilerMessage(infoLogLength);
            glGetProgramInfoLog(programID, infoLogLength, nullptr, compilerMessage.data());
            std::cout << "ERROR: " << compilerMessage.data() << std::endl;
        }

        glUseProgram(programID);

        glDeleteShader(vsID);
        glDeleteShader(fsID);
        glDeleteShader(tcsID);
        glDeleteShader(tesID);

        return programID;
    }

    GLuint CreateProgram(const char* vsSource, const char* fsSource, const char* tcsSource, const char* tesSource, const char* gsSource) {
        GLuint vsID = CompileShader(vsSource, GL_VERTEX_SHADER);
        GLuint fsID = CompileShader(fsSource, GL_FRAGMENT_SHADER);
        GLuint tcsID = CompileShader(tcsSource, GL_TESS_CONTROL_SHADER);
        GLuint tesID = CompileShader(tesSource, GL_TESS_EVALUATION_SHADER);
        GLuint gsID = CompileShader(gsSource, GL_GEOMETRY_SHADER);

        GLuint programID = glCreateProgram();
        glAttachShader(programID, vsID);
        glAttachShader(programID, fsID);
        glAttachShader(programID, tcsID);
        glAttachShader(programID, tesID);
        glAttachShader(programID, gsID);

        glLinkProgram(programID);
        GLint result = GL_FALSE;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        int infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 1) {
            std::vector<char> compilerMessage(infoLogLength);
            glGetProgramInfoLog(programID, infoLogLength, nullptr, compilerMessage.data());
            std::cout << "ERROR: " << compilerMessage.data() << std::endl;
        }

        glUseProgram(programID);

        glDeleteShader(vsID);
        glDeleteShader(fsID);
        glDeleteShader(tcsID);
        glDeleteShader(tesID);
        glDeleteShader(gsID);

        return programID;
    }

    void UpdateLightPos(glm::vec3 lightPos, glm::mat4 mv, GLuint program) {
        glm::vec3 lightPosCy = glm::vec3(mv * glm::vec4(lightPos.x, lightPos.y, lightPos.z, 0));
        if (program > 0) {
            glUseProgram(program);
            glUniform3f(glGetUniformLocation(program, "lightPos"), lightPosCy.x, lightPosCy.y, lightPosCy.z);
        }
    }

    void SetCubemap(int textureUnit) {
        GLuint texID;
        glGenTextures(1, &texID);
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

        int width, height, nrChannels;
        unsigned char* data;
        const char* textures_faces[6] = { "res/cubemap/cubemap_posx.png", "res/cubemap/cubemap_negx.png", "res/cubemap/cubemap_posy.png",
                                        "res/cubemap/cubemap_negy.png", "res/cubemap/cubemap_posz.png", "res/cubemap/cubemap_negz.png" };
        for (unsigned int i = 0; i < 6; i++)
        {
            data = stbi_load(textures_faces[i], &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
            }
            else {
                printf("Failed to load texture %s\n", textures_faces[i]);
            }
        }
    }

    void SetSkyboxMaterial(int textureUnit, GLuint backgroundProgram = -1, GLuint reflectiveProgram = -1) {
        printf("Setting skybox\n");
        SetCubemap(textureUnit);

        if (backgroundProgram > 0) {
            glUseProgram(backgroundProgram);
            glUniform1i(glGetUniformLocation(backgroundProgram, "skybox"), textureUnit);
            glUniform1i(glGetUniformLocation(backgroundProgram, "skyboxSet"), 1);
        }
        if (reflectiveProgram > 0) {
            glUseProgram(reflectiveProgram);
            glUniform1i(glGetUniformLocation(reflectiveProgram, "skybox"), textureUnit);
            glUniform1i(glGetUniformLocation(reflectiveProgram, "skyboxSet"), 1);
        }
    }

    void BindTexture(int textureUnit, unsigned int texture, int width, int height, unsigned char* data) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void SetTexture(const char* filename, int textureUnit) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
        if (data) {
            unsigned int texture;
            glGenTextures(1, &texture);
            BindTexture(textureUnit, texture, width, height, data);
        }
        else {
            printf("Failed to load texture %s\n", filename);
        }
        stbi_image_free(data);
    }

    int SetTexturesData(sg::Model model, GLuint programID, int startingTexture = 0) {
        glUseProgram(programID);
        for (int i = 0; i < model.GetNMaterials(); i++) {
            printf("Adding texture, current index: %d\n", startingTexture);
            sg::Material* mat = model.GetMaterialReferenceAt(i);
            if (mat->texture_Kd.isPresent) {
                SetTexture(mat->texture_Kd.map, startingTexture);
                mat->texture_Kd.index = startingTexture;
                startingTexture++;
            }
            if (mat->texture_Ks.isPresent) {
                SetTexture(mat->texture_Ks.map, startingTexture);
                mat->texture_Ks.index = startingTexture;
                startingTexture++;
            }
            if (mat->texture_Ka.isPresent) {
                SetTexture(mat->texture_Ka.map, startingTexture);
                mat->texture_Ka.index = startingTexture;
                startingTexture++;
            }
        }
        return startingTexture;
    }

    void SetMaterialData(GLuint programId, sg::Material mat) {
        glUseProgram(programId);
        GLint Kd = glGetUniformLocation(programId, "Kd");
        glUniform3f(Kd, mat.Kd[0], mat.Kd[1], mat.Kd[2]);
        GLint Ks = glGetUniformLocation(programId, "Ks");
        glUniform3f(Ks, mat.Ks[0], mat.Ks[1], mat.Ks[2]);
        GLint Ka = glGetUniformLocation(programId, "Ka");
        glUniform3f(Ka, mat.Ka[0], mat.Ka[1], mat.Ka[2]);
        GLint Ns = glGetUniformLocation(programId, "Ns");
        glUniform1f(Ns, mat.Ns);
        GLint d = glGetUniformLocation(programId, "d");
        glUniform1f(d, mat.d);
        if (mat.texture_Kd.isPresent) {
            glUniform1i(glGetUniformLocation(programId, "dTexture"), mat.texture_Kd.index);
            glUniform1i(glGetUniformLocation(programId, "dTextureSet"), 1);
        }
        else {
            glUniform1i(glGetUniformLocation(programId, "dTextureSet"), 0);
        }
        if (mat.texture_Ks.isPresent) {
            glUniform1i(glGetUniformLocation(programId, "sTexture"), mat.texture_Ks.index);
            glUniform1i(glGetUniformLocation(programId, "sTextureSet"), 1);
        }
        else {
            glUniform1i(glGetUniformLocation(programId, "sTextureSet"), 0);
        }
        if (mat.texture_Ka.isPresent) {
            glUniform1i(glGetUniformLocation(programId, "aTexture"), mat.texture_Ka.index);
            glUniform1i(glGetUniformLocation(programId, "aTextureSet"), 1);
        }
        else {
            glUniform1i(glGetUniformLocation(programId, "aTextureSet"), 0);
        }
    }

    void SetMaterialData(GLuint programId) {
        glUseProgram(programId);
        glUniform3f(glGetUniformLocation(programId, "Kd"), 0.4, 0.4, 0.4);
        glUniform3f(glGetUniformLocation(programId, "Ks"), 0.4, 0.4, 0.4);
        glUniform3f(glGetUniformLocation(programId, "Ka"), 0.6, 0.6, 0.6);
        glUniform1f(glGetUniformLocation(programId, "Ns"), 20);
        glUniform1f(glGetUniformLocation(programId, "d"), 1);

        glUniform1i(glGetUniformLocation(programId, "dTextureSet"), 0);
        glUniform1i(glGetUniformLocation(programId, "sTextureSet"), 0);
        glUniform1i(glGetUniformLocation(programId, "aTextureSet"), 0);
    }

    double getCurrentTimeMillis() {
        return std::chrono::system_clock::now().time_since_epoch().count() / 1000;
    }

    void SetMatrix(glm::mat4 matrix, GLuint program, char const* name) {
        GLint location = glGetUniformLocation(program, name);
        glUseProgram(program);
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix));
    }

    void SetMatrix(glm::mat3 matrix, GLuint program, char const* name) {
        GLint location = glGetUniformLocation(program, name);
        glUseProgram(program);
        glUniformMatrix3fv(location, 1, false, glm::value_ptr(matrix));
    }
}