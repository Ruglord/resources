#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <iostream>

#include <vector>
#include "glew.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"



int loadShaders(const GLchar* source, GLenum shaderType );
bool vecIntersect(glm::vec4& vec1,glm::vec4& vec2);
bool vecContains(glm::vec4 r1, glm::vec4 r2); //returns true if r2 contains r1
bool pointInVec(const glm::vec4& vec1, double x, double y, double angle = 0); //angle of vec1 is by default 0
bool lineInVec(const glm::vec2& p1, const glm::vec2& p2, const  glm::vec4& r1, double angle = 0); //angle of r1 is by default 0
class RenderProgram;
void drawLine(RenderProgram& program,glm::vec3 color,const std::vector<std::pair<glm::vec2,glm::vec2>>& points);
glm::vec2 rotatePoint(const glm::vec2& p, const glm::vec2& rotateAround, double angle);
void drawRectangle(RenderProgram& program, const glm::vec3& color, const glm::vec4& rect, double angle);


class RenderProgram
{
    unsigned int program;
public:
    RenderProgram(std::string vertexPath, std::string fragmentPath);
    RenderProgram()
    {

    }
    void setMatrix4fv(std::string name, const GLfloat* value);
    void setVec3fv(std::string name,glm::vec3 value);
    void setVec4fv(std::string name, glm::vec4 value);
    void use();
    void init(std::string vertexPath, std::string fragmentPath);
};

struct SpriteParameter //stores a bunch of information regarding how to render the sprite
{
     glm::vec4 rect = {0,0,0,0};
    float radians = 0;
    glm::vec3 tint = {1,1,1};
    glm::vec4 portion = {0,0,1,1};

};

class Sprite
{
    int width = 0, height = 0;
    int screenWidth = 0, screenHeight = 0;
    unsigned int texture = -1;
    float values[16] = {
    -1, 1, 0, 1,
    1, 1, 1, 1,
    -1, -1, 0, 0,
    1, -1, 1, 0

    };
    float modified[16] = { //values except not const, so it can be modified
    -1, 1, 0, 1,
    1, 1, 1, 1,
    -1, -1, 0, 0,
    1, -1, 1, 0

    };
    int indices[6] = {
    0,1,3,
    0,2,3


    };
    glm::vec3 tint;
    unsigned int VBO=-1, VAO=-1;
    void load(std::string source,bool transparent);
public:
    Sprite(int stanWidth, int stanHeight, std::string source, bool transparent);
    Sprite()
    {
        texture = -1;
    }
    void init(int stanWidth, int stanHeight, std::string source, bool transparent);
    void loadBuffers() const;
  //  virtual void render(RenderProgram& program, SpriteParameter parameter);
    void setPortion(double x, double y, double w, double h );
    void mirror();
    void flip();
    virtual void renderInstanced(RenderProgram& program, const std::vector<SpriteParameter>& parameters);
    unsigned int getVAO();
    void reset(); //clears all buffers and resets modified back to values
    void setTint(glm::vec3 color);
};

class Sprite9 : public Sprite // This sprite has been split into 9 sections that each scale differently.
{

    glm::vec2 widths; //the widths of the frame on either side;
    glm::vec2 heights; //heights of the frame on either side;
public:
    Sprite9(int stanWidth, int stanHeight, std::string source, bool transparent, glm::vec2 W, glm::vec2 H);
    Sprite9()
    {

    }
    void init(int stanWidth, int stanHeight, std::string source, bool transparent, glm::vec2 W, glm::vec2 H);
    void renderInstanced(RenderProgram& program, const std::vector<SpriteParameter>& parameters);

};

#endif // SPRITES_H_INCLUDED
