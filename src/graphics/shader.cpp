#include "graphics/shader.h"
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <glad/glad.h>

namespace gfx
{
    std::string readFile(const std::string& path)
    {
        std::string result;
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            file.open(path);
            std::stringstream stream;
            stream << file.rdbuf();
            file.close();
            result = stream.str();
        }
        catch (std::ifstream::failure& e)
        {
            spdlog::error("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: {}", e.what());
        }
        return result;
    }

    Shader::Shader(const std::string& vertPath, const std::string& fragPath, const std::string& geomPath)
    {
        loadFromFile(vertPath, fragPath, geomPath);
    }

    Shader::~Shader()
    {
        destroy();
    }

    Shader::Shader(Shader &&other) noexcept
        : m_id(other.m_id), m_uniformLocations(other.m_uniformLocations)
    {
        other.m_id = 0;
    }

    Shader &Shader::operator=(Shader &&other) noexcept
    {
        if (this != &other)
        {
            m_id = other.m_id;
            other.m_id = 0;
            m_uniformLocations = other.m_uniformLocations;
        }
        return *this;
    }

    void Shader::loadFromFile(const std::string& vertPath, const std::string& fragPath, const std::string& geomPath)
    {
        std::string vertString = readFile(vertPath);
        const char* vShaderCode = vertString.c_str();
        std::string fragString = readFile(fragPath);
        const char* fShaderCode = fragString.c_str();
        std::string geomString;
        const char* gShaderCode;
        if (!geomPath.empty()) {
            geomString = readFile(vertPath);
            gShaderCode = geomString.c_str();
        }

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        compileShader(vertexShader);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        compileShader(fragmentShader);
        GLuint geometryShader;
        if (!geomPath.empty()) {
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShader, 1, &gShaderCode, NULL);
            compileShader(geometryShader);
        }

        m_id = glCreateProgram();
        glAttachShader(m_id, vertexShader);
        glAttachShader(m_id, fragmentShader);
        if (!geomPath.empty())
            glAttachShader(m_id, geometryShader);
        linkProgram(m_id);

        glDetachShader(m_id, vertexShader);
        glDetachShader(m_id, fragmentShader);
        if (!geomPath.empty())
            glDetachShader(m_id, geometryShader);
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (!geomPath.empty())
            glDeleteShader(geometryShader);
        
        m_uniformLocations = std::unordered_map<std::string, int>();
        int numUniforms;
        glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &numUniforms);
        for (int i = 0; i < numUniforms; i++)
        {
            char name[256];
            int length, size;
            GLenum type;
            glGetActiveUniform(m_id, i, 256, &length, &size, &type, name);
            m_uniformLocations[std::string(name)] = glGetUniformLocation(m_id, name);
        }
    }

    void Shader::use()
    {
        glUseProgram(m_id);
    }

    void Shader::destroy()
    {
        if (m_id)
            glDeleteProgram(m_id);
        m_id = 0;
    }

    unsigned int Shader::getID()
    {
        return m_id;
    }

    int Shader::getAttribLocation(const char *name)
    {
        return glGetAttribLocation(m_id, name);
    }

    void Shader::setBool(const char *name, bool value)
    {
        glUseProgram(m_id);
        glUniform1i(m_uniformLocations[name], (int)value);
    }

    void Shader::setInt(const char *name, int value)
    {
        glUseProgram(m_id);
        glUniform1i(m_uniformLocations[name], value);
    }

    void Shader::setFloat(const char *name, float value)
    {
        glUseProgram(m_id);
        glUniform1f(m_uniformLocations[name], value);
    }

    void Shader::setVec2(const char *name, const glm::vec2 &value)
    {
        glUseProgram(m_id);
        glUniform2fv(m_uniformLocations[name], 1, &value[0]);
    }

    void Shader::setVec3(const char *name, const glm::vec3 &value)
    {
        glUseProgram(m_id);
        glUniform3fv(m_uniformLocations[name], 1, &value[0]);
    }

    void Shader::setVec4(const char *name, const glm::vec4 &value)
    {
        glUseProgram(m_id);
        glUniform4fv(m_uniformLocations[name], 1, &value[0]);
    }

    void Shader::setMat2(const char *name, const glm::mat2 &value)
    {
        glUseProgram(m_id);
        glUniformMatrix2fv(m_uniformLocations[name], 1, GL_FALSE, &value[0][0]);
    }

    void Shader::setMat3(const char *name, const glm::mat3 &value)
    {
        glUseProgram(m_id);
        glUniformMatrix3fv(m_uniformLocations[name], 1, GL_FALSE, &value[0][0]);
    }

    void Shader::setMat4(const char *name, const glm::mat4 &value)
    {
        glUseProgram(m_id);
        glUniformMatrix4fv(m_uniformLocations[name], 1, GL_FALSE, &value[0][0]);
    }

    void Shader::compileShader(int shader)
    {
        glCompileShader(shader);
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            spdlog::error("ERROR::SHADER::COMPILATION_FAILED: {}", infoLog);
        }
    }

    void Shader::linkProgram(int program)
    {
        glLinkProgram(program);
        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            spdlog::error("ERROR::SHADER::PROGRAM::LINKING_FAILED: {}", infoLog);
        }
    }
}
