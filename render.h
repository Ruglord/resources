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
bool lineInLine(const glm::vec2& a1, const glm::vec2& a2, const glm::vec2& b1, const glm::vec2& b2);
class RenderProgram;
void drawLine(RenderProgram& program,glm::vec3 color,const std::vector<glm::vec4>& points);
glm::vec2 rotatePoint(const glm::vec2& p, const glm::vec2& rotateAround, double angle);
void drawRectangle(RenderProgram& program, const glm::vec3& color, const glm::vec4& rect, double angle);

class RenderProgram
{
    unsigned int program;
public:
    static RenderProgram basicProgram, lineProgram; //basic allows for basic sprite rendering. Line program is simpler and renders lines.
    RenderProgram(std::string vertexPath, std::string fragmentPath);
    RenderProgram()
    {

    }
    void setMatrix4fv(std::string name, const GLfloat* value);
    void setVec3fv(std::string name,glm::vec3 value);
    void setVec4fv(std::string name, glm::vec4 value);
    void use();
    void init(std::string vertexPath, std::string fragmentPath);
    static void init(int screenWidth, int screenHeight); //this init function initiates the basic renderprograms
};

//extern RenderProgram RenderProgram::basicProgram;
//extern RenderProgram RenderProgram::lineProgram;

struct SpriteParameter //stores a bunch of information regarding how to render the sprite
{
     glm::vec4 rect = {0,0,0,0};
    float radians = 0;
    glm::vec3 tint = {1,1,1};
    glm::vec4 portion = {0,0,1,1};
    std::vector<float> modified = { //values except not const, so it can be modified
    -1, 1, 0, 1,
    1, 1, 1, 1,
    -1, -1, 0, 0,
    1, -1, 1, 0
    };
    std::vector<int> modIndices= {
    0, 1, 3,
    0, 2, 3};
};

class Sprite
{
protected:
    int width = 0, height = 0;
    unsigned int texture = -1;
    float values[16] = {
    -1, 1, 0, 1,
    1, 1, 1, 1,
    -1, -1, 0, 0,
    1, -1, 1, 0

    };
    std::vector<float> modified = { //values except not const, so it can be modified
    -1, 1, 0, 1,
    1, 1, 1, 1,
    -1, -1, 0, 0,
    1, -1, 1, 0
    };
    int indices[6] = {
    0,1,3,
    0,2,3
    };
    std::vector<int> modIndices= {
    0, 1, 3,
    0, 2, 3};
    glm::vec3 tint;
    unsigned int VBO=-1, VAO=-1;
    void load(std::string source,bool transparent);
public:
    Sprite(std::string source, bool transparent);
    Sprite()
    {
        texture = -1;
    }
    void init(std::string source, bool transparent);
    void loadBuffers() const;
  //  virtual void render(RenderProgram& program, SpriteParameter parameter);
    virtual void renderInstanced(RenderProgram& program, const std::vector<SpriteParameter>& parameters);
    unsigned int getVAO();
    void reset(); //clears all buffers and resets modified back to values
    void setTint(glm::vec3 color);
    void mirror();
    void flip();
   // void map(RenderProgram& program,double width, double height,const glm::vec4& base, const std::vector<glm::vec2>& points);
};

class Sprite9 : public Sprite // This sprite has been split into 9 sections that each scale differently. The corners aren't scaled at all, the top and bottom
{                               //are only scaled horizontally, the sides are only scaled vertically, and the center can be centered any which way.

    glm::vec2 widths; //the widths of the frame on either side;
    glm::vec2 heights; //heights of the frame on either side;
public:
    Sprite9(std::string source, bool transparent, glm::vec2 W, glm::vec2 H);
    Sprite9()
    {

    }
    void init(std::string source, bool transparent, glm::vec2 W, glm::vec2 H);
    void renderInstanced(RenderProgram& program, const std::vector<SpriteParameter>& parameters);

};

struct AnimationParameter //the main difference between this class and SpriteParameters is that this one doesn't provide portion info and instead the time at which the animation started
{
    glm::vec4 rect = {0,0,0,0};
    double start = -1; //the time at which the animation started, -1 if it hasn't started
    double fps = -1; //the fps for the animation. -1 means use the default
    float radians = 0;
    glm::vec3 tint = {1,1,1};
};

class BaseAnimation : public Sprite //the actual animation object
{
    double fps = 0;
    glm::vec2 frameDimen;
public:
    BaseAnimation(std::string source, bool transparent, double speed, int perRow, int rows);
    BaseAnimation()
    {

    }
    void init(std::string source, bool transparent, double speed, int perRow, int rows); //how many frames per row and how many rows there are
    void renderInstanced(RenderProgram& program, const std::vector<AnimationParameter>& parameters);
};
#endif // SPRITES_H_INCLUDED
