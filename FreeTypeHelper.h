#ifndef FREETYPEHELPER_H_INCLUDED
#define FREETYPEHELPER_H_INCLUDED

#include "glew.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <map>
#include "render.h"


class Character //represents a character in a font

{
public:
    char letter;
    GLuint texture = -1;
    glm::vec2 size; //the dimensions of the character
    glm::vec2 bearing; //the bearing, or margins of the character
    GLuint advance; //the total width the character takes up, including the character width, the horizontal bearing, and the space from the next character
};


class Font
{
    std::map<GLchar,Character> characters;
    std::string font = "";
    GLuint  VBO,VAO;
    int stanW = 0;
    int stanH = 0;
    int indices[6] = {
        0,1,2,
        3,1,2};
public:
    Font(std::string source, int w,int h);
    Font()
    {

    }
    void init(std::string source, int w, int h);
    glm::vec2 getDimen(std::string text, GLfloat scale); //gets the dimensions on text printed if the text were to be printed.
    glm::vec2 write(RenderProgram& p,std::string shadeName, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
    ~Font()
    {
        characters.clear();
    }
    };
#endif // FREETYPEHELPER_H_INCLUDED
