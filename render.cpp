#include <sstream>
#include <fstream>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "SDL.h"

#include "render.h"


bool vecIntersect(glm::vec4& vec1,glm::vec4& vec2)
{
    return (vec1.x <= vec2.x + vec2.z && vec1.x + vec1.z>= vec2.x && vec1.y <= vec2.y + vec2.a && vec1.y + vec1.a >= vec2.y);
}

bool pointInVec(glm::vec4 vec1, double x, double y)
{
    return (vec1.x <= x && vec1.x + vec1.z >= x) && (vec1.y <= y && vec1.y + vec1.a >= y);
}

bool vecContains(glm::vec4 r1, glm::vec4 r2)
{
    return (r1.x > r2.x && r1.x + r1.z < r2.x + r2.z && r1.y > r2.y && r1.y + r1.a < r2.y + r2.a);
}

int loadShaders(const GLchar*source, GLenum shaderType )
{
    std::ifstream input;
    input.open(source);
    int shader = -1;
    if (input.is_open())
    {

    std::stringstream stream;
    stream << input.rdbuf();
     shader = glCreateShader(shaderType);
     std::string str = stream.str();
    const char* code = str.c_str();
    glShaderSource(shader,1, &code, NULL);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Error loading shader " << "source: " << infoLog << std::endl;
    }
    }
    else
    {
        std::cout << "Can't open shader! Source: " << source << std::endl;
    }
    return shader;

}

void drawLine(RenderProgram& program, glm::vec3 color, const std::vector<std::pair<glm::vec2,glm::vec2>>& points)
{
    int size = points.size();
    GLfloat buffer_data[size*6];/* = {points[0].first.x, points[0].first.y, 0,
    points[0].second.x, points[0].second.y, 0,
    points[1].first.x, points[1].first.y, 0,
    points[1].second.x, points[1].second.y,0};*/
    for (int i = 0;i < size*6; i ++)
    {
        int modI = i/6;
        buffer_data[i] = points[modI].first.x;
        buffer_data[i+1] = points[modI].first.y;
        buffer_data[i+2] = 0;
        buffer_data[i+3] = points[modI].second.x;
        buffer_data[i+4] = points[modI].second.y;
        buffer_data[i+5] = 0;
        i+=5;
    }
    GLuint VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
GLuint VBO;
glGenBuffers(1,&VBO);
glBindBuffer(GL_ARRAY_BUFFER,VBO);
glBufferData(GL_ARRAY_BUFFER,sizeof(buffer_data),buffer_data,GL_STATIC_DRAW);
 glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
glEnableVertexAttribArray(0);
    program.setVec3fv("color",color);

    program.use();
    glDrawArraysInstanced(GL_LINES,0,4,2);

}
RenderProgram::RenderProgram(std::string vertexPath, std::string fragmentPath)
{
    init(vertexPath,fragmentPath);
}
void RenderProgram::init(std::string vertexPath, std::string fragmentPath)
{
    GLuint fragment = -1, vertex= -1;
    program = glCreateProgram();
    vertex = loadShaders(vertexPath.c_str(), GL_VERTEX_SHADER );
    fragment = loadShaders(fragmentPath.c_str(),GL_FRAGMENT_SHADER);
    glAttachShader(program,vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(fragment);
    glDeleteShader(vertex);
}
void RenderProgram::use()
{
    glUseProgram(program);
}
void RenderProgram::setMatrix4fv(std::string name, const GLfloat* value)
{
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program,name.c_str()),1,GL_FALSE,value);
    glUseProgram(0);
}
void RenderProgram::setVec3fv(std::string name,glm::vec3 value)
{
    glUseProgram(program);
    glUniform3fv(glGetUniformLocation(program,name.c_str()),1,glm::value_ptr(value));
    glUseProgram(0);
}
void RenderProgram::setVec4fv(std::string name,glm::vec4 value)
{
    glUseProgram(program);
    glUniform4fv(glGetUniformLocation(program,name.c_str()),1,glm::value_ptr(value));
    glUseProgram(0);
}
   void Sprite::load(std::string source,bool transparent)
    {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1,&VBO);

        glBindVertexArray(VAO);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D,texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int channels;

        unsigned char* data = stbi_load(source.c_str(),&width, &height, &channels, 0);

        int rgb = GL_RGB*!transparent + GL_RGBA*transparent;
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, rgb,width, height, 0, rgb, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Error loading texture: " << source << std::endl;
        }
        stbi_image_free(data);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

Sprite::Sprite(int stanWidth, int stanHeight, std::string source, bool transparent)
    {
        init(stanWidth,stanHeight,source,transparent);
    }
void Sprite::setTint(glm::vec3 color)
{
    tint = color;
}
void Sprite::init(int stanWidth, int stanHeight, std::string source, bool transparent)
    {
        load(source, transparent);
        screenWidth = stanWidth;
        screenHeight = stanHeight;
    }
void Sprite::loadBuffers() const
{
           //glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(modified), modified, GL_STATIC_DRAW);
        glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
            glEnableVertexAttribArray(0);
}
void Sprite::setPortion(double x, double y, double w, double h) //sets the portion. Assumes that the 4 variables are STANDARDIZED
{
    glm::vec4 portion = {x,y,w,h};
    for (int i = 0; i < 8; ++i)
    {
        modified[i%2+2 + i/2*4] = portion.x*(i == 0 || i == 4) + portion.y*(i == 1 || i == 3) + (portion.x + portion.z)*(i == 2 || i == 6) + (portion.y + portion.a)*(i == 5 || i == 7);
    }
}
void Sprite::mirror()
{
    for (int i = 0; i < 4;++i)
    {
        int first = i*4+2;
        modified[first] = ((int)modified[first]+1)%2;
    }
}
void Sprite::flip() //flips the sprite vertically
{
    for (int i = 0; i < 4;++i)
    {
        int first = i*4+3;
        modified[first] = ((int)modified[first]+1)%2;
    }
}
void Sprite::reset()
{
    glBindVertexArray(0);
glBindBuffer(GL_ARRAY_BUFFER,0);
    for (int i = 0; i < 16; i ++)
    {
        modified[i] = values[i];
    }
    tint = {1,1,1};
}

void Sprite::renderInstanced(RenderProgram& program, const std::vector<SpriteParameter>& parameters)
{
       // if (true)
    {
        program.use();
  //  std::cout << length << std::endl;
    int size = parameters.size();
    glm::mat4 matrices[size];
    glm::vec3 tints[size];
    glm::vec4 portions[size];
 //   std::cout << scale.x<< scale.y << std::endl;
   // glm::vec3 adjust(-(1-scale.x), 1- (scale.y),0);
    //std::cout << adjust.x << " " << adjust.y << std::endl;
    for (int i = 0; i < size; i ++)
    {
        const SpriteParameter* current = &(*(parameters.begin() + i));

     //   std::cout << current.x << " " << current.y << std::endl;
        matrices[i] = glm::mat4(1.0f);
        matrices[i] = glm::translate(matrices[i],{current->rect.x + current->rect.z/2,current->rect.y + current->rect.a/2,0});
        matrices[i] = glm::rotate(matrices[i], current->radians, glm::vec3(0,0,1));
        matrices[i] = glm::scale(matrices[i], {current->rect.z/2, current->rect.a/2,1});
        tints[i] = current->tint;
        portions[i] = current->portion;
   }
    glBindVertexArray(VAO);
    unsigned int transform;
    glGenBuffers(1,&transform);
    glBindBuffer(GL_ARRAY_BUFFER,transform);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::mat4)*size,matrices,GL_STATIC_DRAW);
GLsizei vec4Size = sizeof(glm::vec4);
glEnableVertexAttribArray(3);
glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
glEnableVertexAttribArray(4);
glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
glEnableVertexAttribArray(5);
glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
glEnableVertexAttribArray(6);
glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

glVertexAttribDivisor(3, 1);
glVertexAttribDivisor(4, 1);
glVertexAttribDivisor(5, 1);
glVertexAttribDivisor(6, 1);


unsigned int colors;
glGenBuffers(1,&colors);
glBindBuffer(GL_ARRAY_BUFFER, colors);
glBufferData(GL_ARRAY_BUFFER, sizeof(tints), tints, GL_STATIC_DRAW);
glEnableVertexAttribArray(2);
glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);
glVertexAttribDivisor(2,1);


unsigned int parts;
glGenBuffers(1,&parts);
glBindBuffer(GL_ARRAY_BUFFER, parts);
glBufferData(GL_ARRAY_BUFFER, sizeof(portions),portions, GL_STATIC_DRAW);
glEnableVertexAttribArray(7);
glVertexAttribPointer(7,4,GL_FLOAT,GL_FALSE,0,0);
glVertexAttribDivisor(7,1);


loadBuffers();

glBindTexture(GL_TEXTURE_2D,texture);
glDrawElementsInstanced(GL_TRIANGLES,6,GL_UNSIGNED_INT,&indices,size);
glBindVertexArray(0);
glBindBuffer(GL_ARRAY_BUFFER,0);
reset();
glDeleteBuffers(1,&transform);

glDeleteBuffers(1,&colors);

glDeleteBuffers(1,&parts);

    }
  /*  else
    {
        std::cout << "Haven't loaded a texture yet!" << std::endl;
    }*/
}

unsigned int Sprite::getVAO()
{
    return VAO;
}

Sprite9::Sprite9(int stanWidth, int stanHeight, std::string source, bool transparent, glm::vec2 W, glm::vec2 H) : Sprite(stanWidth, stanHeight, source, transparent)
{
    widths = W;
    heights = H;
}
void Sprite9::init(int stanWidth, int stanHeight, std::string source, bool transparent, glm::vec2 W, glm::vec2 H)
{
    Sprite::init(stanWidth,stanHeight,source, transparent);
    widths = W;
    heights = H;
}
void Sprite9::renderInstanced(RenderProgram& program, const std::vector<SpriteParameter>& parameters)
{
    std::vector<SpriteParameter> portions;
    int size = parameters.size();
    for (int i = 0; i < size; i ++)
    {
        const SpriteParameter* current = &parameters[i];
        glm::vec4 rect = current->rect;

        for ( int g = 0; g < 3; g ++)
        {
            float horiz = (rect.z-widths.x-widths.y); //the width of the scalable parts.
            float vert = rect.a- heights.x-heights.y; //the height of the scalable parts
            for (int h = 0; h < 3; h ++)
            {
                portions.push_back({{rect.x + widths.x*(h > 0) + (horiz)*(h>1),rect.y + heights.x*(g>0) + (vert)*(g>1), widths.x*(h == 0) + horiz*(h==1)+widths.y*(h==2), heights.x*(g==0) + vert * (g == 1) + heights.y*(g==2)},
                                   0,current->tint,{1.0/3*h,(1.0/3)*g,1.0/3,1.0/3}});
            }
        }
    }
    Sprite::renderInstanced(program, portions);
}
