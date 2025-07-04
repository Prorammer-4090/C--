#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include<iostream>

class Shader{
    public:
        // the program ID
        unsigned int ID;

        // Constructor reads and builds the shader
        Shader(const char* vertexPath, const char* fragmentPath);
        // use/activate the shader
        void use();
        // utility uniforms functions
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
};

#endif