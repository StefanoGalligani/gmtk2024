#pragma once
#include <sgStructures.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace sg {
    class TextureManager {
    private:
        static TextureManager _instance;
        static bool _initialized;
        std::vector<Texture> _loadedTextures;

        TextureManager() {
            
        }

    public:

        static TextureManager* Instance();

	private:
        GLuint CheckIfAlreadyLoaded(const char* filename) {
            for (int i = 0; i < _loadedTextures.size(); i++) {
                char* name = _loadedTextures[i].map;
                int j = 0;
                bool found = true;
                while (filename[j] != '\0') {
                    if (name[j] != filename[j]) {
                        found = false;
                        break;
                    }
                    j++;
                }
                if (found && name[j] == '\0') {
                    return _loadedTextures[i].index;
                }
            }
            return -1;
        }

        void BindTexture(GLuint texture, int width, int height, unsigned char* data) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

    public:
        GLuint SetTexture(const char* filename) {
            int width, height, nrChannels;
            unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, STBI_rgb_alpha);
            GLuint texture = -1;
            if (data) {
                glGenTextures(1, &texture);
                BindTexture(texture, width, height, data);
            }
            else {
                printf("Failed to load texture %s\n", filename);
            }
            stbi_image_free(data);
            return texture;
        }

        void SetTexturesData(sg::Material* mat) {
            if (mat->texture_Kd.isPresent && !mat->texture_Kd.isLoaded) {
                GLuint index = CheckIfAlreadyLoaded(mat->texture_Kd.map);
                bool wasPresent = index != -1;
                if (!wasPresent) {
                    index = SetTexture(mat->texture_Kd.map);
                }
                mat->texture_Kd.index = index;
                mat->texture_Kd.isLoaded = true;
                if (!wasPresent) {
                    _loadedTextures.push_back(mat->texture_Kd);
                }
            }
            if (mat->texture_Ks.isPresent && !mat->texture_Ks.isLoaded) {
                GLuint index = CheckIfAlreadyLoaded(mat->texture_Ks.map);
                bool wasPresent = index != -1;
                if (!wasPresent) {
                    index = SetTexture(mat->texture_Ks.map);
                }
                mat->texture_Ks.index = index;
                mat->texture_Ks.isLoaded = true;
                if (!wasPresent) {
                    _loadedTextures.push_back(mat->texture_Ks);
                }
            }
        }

        GLuint SetCubemap(const char* textures_faces[6]) {
            GLuint texID;
            glGenTextures(1, &texID);
            glActiveTexture(GL_TEXTURE0);
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
            for (unsigned int i = 0; i < 6; i++)
            {
                data = stbi_load(textures_faces[i], &width, &height, &nrChannels, 0);
                if (data) {
                    glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                    );
                    //free(data); TODO dopo che funziona provare a rimetterlo
                }
                else {
                    printf("Failed to load texture %s\n", textures_faces[i]);
                }
            }
            return texID;
        }

        void SetMaterialData(GLuint programId, Material* mat) {
            SetTexturesData(mat);
            glUseProgram(programId);
            GLint Kd = glGetUniformLocation(programId, "material.Kd");
            glUniform3f(Kd, mat->Kd[0], mat->Kd[1], mat->Kd[2]);
            GLint Ks = glGetUniformLocation(programId, "material.Ks");
            glUniform3f(Ks, mat->Ks[0], mat->Ks[1], mat->Ks[2]);
            GLint Ns = glGetUniformLocation(programId, "material.Ns");
            glUniform1f(Ns, mat->Ns);
            GLint d = glGetUniformLocation(programId, "material.d");
            glUniform1f(d, mat->d);
            if (mat->texture_Kd.isPresent) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mat->texture_Kd.index);
                glUniform1i(glGetUniformLocation(programId, "material.dTexture"), 0);
                glUniform1i(glGetUniformLocation(programId, "material.dTextureSet"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(programId, "material.dTextureSet"), 0);
            }
            if (mat->texture_Ks.isPresent) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mat->texture_Ks.index);
                glUniform1i(glGetUniformLocation(programId, "material.sTexture"), 1);
                glUniform1i(glGetUniformLocation(programId, "material.sTextureSet"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(programId, "material.sTextureSet"), 0);
            }
        }

        void SetMaterialData(GLuint programId) {
            glUseProgram(programId);
            glUniform3f(glGetUniformLocation(programId, "material.Kd"), 1, 1, 1);
            glUniform3f(glGetUniformLocation(programId, "material.Ks"), 0.4, 0.4, 0.4);
            glUniform1f(glGetUniformLocation(programId, "material.Ns"), 20);
            glUniform1f(glGetUniformLocation(programId, "material.d"), 1);

            glUniform1i(glGetUniformLocation(programId, "material.dTextureSet"), 0);
            glUniform1i(glGetUniformLocation(programId, "material.sTextureSet"), 0);
        }
	};

    TextureManager TextureManager::_instance = TextureManager::TextureManager();
    bool TextureManager::_initialized = false;

    TextureManager* TextureManager::Instance() {
        if (!TextureManager::_initialized) {
            TextureManager::_instance = TextureManager();
            TextureManager::_initialized = true;
        }
        return &TextureManager::_instance;
    }
}