#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace gfx
{
    class Shader
    {
    public:
        Shader() = default;
        Shader(const std::string& vertPath, const std::string& fragPath, const std::string& geomPath=std::string());
        ~Shader();

        Shader(const Shader& other) = delete;
        Shader(Shader&& other);
        Shader& operator=(const Shader& other) = delete;
        Shader& operator=(Shader&& other);

        void loadFromFile(const std::string& vertPath, const std::string& fragPath, const std::string& geomPath=std::string());

        void use();
        void destroy();

        unsigned int getID();

        int getAttribLocation(const char* name);

        void setBool(const char* name, bool value);
        void setInt(const char* name, int value);
        void setFloat(const char* name, float value);
        void setVec2(const char* name, const glm::vec2& value);
        void setVec3(const char* name, const glm::vec3& value);
        void setVec4(const char* name, const glm::vec4& value);
        void setMat2(const char* name, const glm::mat2& value);
        void setMat3(const char* name, const glm::mat3& value);
        void setMat4(const char* name, const glm::mat4& value);


    private:
        static void compileShader(int shader);
        static void linkProgram(int program);

        unsigned int m_id = 0;

        std::unordered_map<std::string, int> m_uniformLocations;
    };
}