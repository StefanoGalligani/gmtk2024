#pragma once

#include <sgModel.h>
#include <GL/glew.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <glm/glm/gtc/type_ptr.hpp>
#include <sgDirectionalLight3D.h>

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

    void UpdateSpotLight(sg::SpotLight3D* spotLight, glm::mat4 mv, GLuint program) {
        if (program <= 0) return;
        glm::vec3 lightPos = glm::vec3(mv * glm::vec4(spotLight->GetGlobalPosition(), 1));
        glUseProgram(program);
        glUniform3fv(glGetUniformLocation(program, "spotLight.pos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(program, "spotLight.color"), 1, glm::value_ptr(spotLight->GetColor()));
        glUniform1f(glGetUniformLocation(program, "spotLight.intensity"), spotLight->GetIntensity());
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, spotLight->GetShadowTexture()); //variare se la texture può essere un rettangolo
        glUniform1i(glGetUniformLocation(program, "spotLight.shadowTexture"), 3);
    }

    void UpdateDirectionalLight(sg::DirectionalLight3D* dirLight, glm::mat4 mv, GLuint program) {
        if (program <= 0) return;
        glm::vec3 lightDir = glm::vec3(mv * glm::vec4(dirLight->GlobalForward(), 0));
        glUseProgram(program);
        glUniform3fv(glGetUniformLocation(program, "dirLight.dir"), 1, glm::value_ptr(lightDir));
        glUniform3fv(glGetUniformLocation(program, "dirLight.color"), 1, glm::value_ptr(dirLight->GetColor()));
        glUniform1f(glGetUniformLocation(program, "dirLight.intensity"), dirLight->GetIntensity());
        glActiveTexture(GL_TEXTURE0 + 4);
        glBindTexture(GL_TEXTURE_2D, dirLight->GetShadowTexture()); //variare se la texture può essere un rettangolo
        glUniform1i(glGetUniformLocation(program, "dirLight.shadowTexture"), 4);
    }

    double getCurrentTimeMillis() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000.0;
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