#pragma once

#include <sgModel.h>
#include <GL/glew.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <glm/glm/gtc/type_ptr.hpp>
#include <sgSpotLight3D.h>
#include <sgPointLight3D.h>
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

    void UpdateSpotLights(GLuint program, std::vector<sg::SpotLight3D*> spotLights, glm::mat4 mv, int textureUnit) {
        if (program <= 0) return;
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "nSpotLights"), spotLights.size());
        for (int i = 0; i < spotLights.size(); i++) {
            glm::vec3 lightPos = glm::vec3(mv * glm::vec4(spotLights[i]->GetGlobalPosition(), 1));
            std::string baseString = std::string("spotLights[").append(std::to_string(i)).append("].");
            glUniform3fv(glGetUniformLocation(program, (baseString + "pos").c_str()), 1, glm::value_ptr(lightPos));
            glUniform3fv(glGetUniformLocation(program, (baseString + "color").c_str()), 1, glm::value_ptr(spotLights[i]->GetColor()));
            glUniform1f(glGetUniformLocation(program, (baseString + "range").c_str()), spotLights[i]->GetRange());
            glUniform1f(glGetUniformLocation(program, (baseString + "intensity").c_str()), spotLights[i]->GetIntensity());
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, spotLights[i]->GetShadowTexture()); //variare se la texture può essere un rettangolo
            glUniform1i(glGetUniformLocation(program, (baseString + "shadowTexture").c_str()), textureUnit);
            textureUnit++;
        }
    }


    void UpdatePointLights(GLuint program, std::vector<sg::PointLight3D*> pointLights, glm::mat4 mv, int textureUnit) {
        if (program <= 0) return;
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "nPointLights"), pointLights.size());
        for (int i = 0; i < pointLights.size(); i++) {
            glm::vec3 lightPos = glm::vec3(mv * glm::vec4(pointLights[i]->GetGlobalPosition(), 1));
            std::string baseString = std::string("pointLights[").append(std::to_string(i)).append("].");
            glUniform3fv(glGetUniformLocation(program, (baseString + "pos").c_str()), 1, glm::value_ptr(lightPos));
            glUniform3fv(glGetUniformLocation(program, (baseString + "color").c_str()), 1, glm::value_ptr(pointLights[i]->GetColor()));
            glUniform1f(glGetUniformLocation(program, (baseString + "range").c_str()), pointLights[i]->GetRange());
            glUniform1f(glGetUniformLocation(program, (baseString + "intensity").c_str()), pointLights[i]->GetIntensity());
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            //glBindTexture(GL_TEXTURE_2D, pointLights[i]->GetShadowTexture());
            //glUniform1i(glGetUniformLocation(program, (baseString + "shadowTexture").c_str()), textureUnit);
            textureUnit++;
        }
    }

    void UpdateDirectionalLights(GLuint program, std::vector<sg::DirectionalLight3D*> dirLights, glm::mat4 mv, int textureUnit) {
        if (program <= 0) return;
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "nDirLights"), dirLights.size());
        for (int i = 0; i < dirLights.size(); i++) {
            glm::vec3 lightDir = glm::vec3(mv * glm::vec4(dirLights[i]->GlobalForward(), 0));
            std::string baseString = std::string("dirLights[").append(std::to_string(i)).append("].");
            glUniform3fv(glGetUniformLocation(program, (baseString + "dir").c_str()), 1, glm::value_ptr(lightDir));
            glUniform3fv(glGetUniformLocation(program, (baseString + "color").c_str()), 1, glm::value_ptr(dirLights[i]->GetColor()));
            glUniform1f(glGetUniformLocation(program, (baseString + "intensity").c_str()), dirLights[i]->GetIntensity());
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, dirLights[i]->GetShadowTexture()); //variare se la texture può essere un rettangolo
            glUniform1i(glGetUniformLocation(program, (baseString + "shadowTexture").c_str()), textureUnit);
            textureUnit++;
        }
    }

    void UpdateAmbientLights(GLuint program, std::vector<sg::AmbientLight*> ambientLights) {
        if (program <= 0) return;
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "nAmbientLights"), ambientLights.size());
        for (int i = 0; i < ambientLights.size(); i++) {
            std::string baseString = std::string("ambientLights[").append(std::to_string(i)).append("].");
            glUniform3fv(glGetUniformLocation(program, (baseString + "color").c_str()), 1, glm::value_ptr(ambientLights[i]->GetColor()));
            glUniform1f(glGetUniformLocation(program, (baseString + "intensity").c_str()), ambientLights[i]->GetIntensity());
        }
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