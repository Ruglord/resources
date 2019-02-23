#include <sstream>
#include <fstream>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "SDL.h"

#include "render.h"

RenderProgram RenderProgram::basicProgram;
RenderProgram RenderProgram::lineProgram;

struct RotateRect
{
    glm::vec4 rect;
    double angle;
};

bool vecIntersect(glm::vec4& vec1,glm::vec4& vec2)
{
    return (vec1.x <= vec2.x + vec2.z && vec1.x + vec1.z>= vec2.x && vec1.y <= vec2.y + vec2.a && vec1.y + vec1.a >= vec2.y);
}

bool pointInVec(const glm::vec4& vec1, double x, double y, double angle)
{
    glm::vec2 center = {vec1.x + vec1.z/2, vec1.y+vec1.a/2};
    glm::vec2 rotated = rotatePoint({x,y},center,-angle);
    return rotated.x >= vec1.x && rotated.x <= vec1.x + vec1.z && rotated.y >= vec1.y && rotated.y <= vec1.y +vec1.a;
}

glm::vec2 rotatePoint(const glm::vec2& p, const glm::vec2& rotateAround, double angle)
{
    glm::vec2 point = {p.x - rotateAround.x,p.y-rotateAround.y};
    return {point.x*cos(angle)-point.y*sin(angle)+rotateAround.x, point.x*sin(angle) + point.y*cos(angle)+rotateAround.y};
}

bool inLine(const glm::vec2& point, const glm::vec2& point1, const glm::vec2& point2) //line 1 and line 2 are the points of the line
{
    if (point1.x == point2.x)
    {
        return point1.x == point.x && point.y >= std::min(point1.y,point2.y) && point.y <= std::max(point1.y,point2.y);
    }
    double slope = (point2.y - point1.y)/(point2.x - point1.x);
    double yInt = point1.y - slope*point1.x;
    return slope*point.x + yInt == point.y && point.x >= std::min(point1.x,point2.x) && point.x <= std::max(point1.x,point2.x);
}

bool lineInLine(const glm::vec2& a1, const glm::vec2& a2, const glm::vec2& b1, const glm::vec2& b2)
{
    glm::vec2 intersect = {0,0};
    glm::vec2 nonVert = {0,0}; //this equals the slope and yInt of the line that is not vertical, or line b1-b2 if neither are vertical.
    if (a1.x - a2.x == 0 && b1.x - b2.x == 0)
    {
        double highest = std::min(a1.y,a2.y);
        return abs(highest - std::min(b1.y,b2.y)) >= abs((highest - (a1.y + a2.y - highest)));
    }
    else
    {
        double slope1 = 0;
        double yInt1 = 0;
        if (a1.x - a2.x != 0)
        {
            slope1 = (a1.y - a2.y)/(a1.x-a2.x);
            yInt1 = a1.y - slope1*a1.x;
            nonVert.x = slope1;
            nonVert.y = yInt1;
        }
        else
        {
            intersect.x = a1.x;
        }
        double slope2 = 0;
        double yInt2 = 0;
        if (b1.x - b2.x != 0)
        {
            slope2 = (b1.y - b2.y)/(b1.x - b2.x);
            yInt2 = b1.y - slope2*b1.x;
            nonVert.x = slope2;
            nonVert.y = yInt2;
        }
        else
        {
            intersect.x = b1.x;
        }
        if (b1.x != b2.x && a1.x != a2.x)
        {
            if (slope1 == slope2)
            {
                if (yInt1 == yInt2)
                {
                    double left = std::min(a1.x, a2.x);
                    double right = std::max(a1.x, a2.x);
                    return (b1.x <= right && b1.x >= left) || (b2.x <= right && b2.x >= left);
                }
                return false;
            }
                intersect.x = (yInt1 - yInt2)/(slope2 - slope1);
        }

    }

    intersect.y = nonVert.x*intersect.x + nonVert.y;
    return intersect.x >= std::min(a1.x, a2.x) && intersect.x <= std::max(a1.x, a2.x) &&
            intersect.x >= std::min(b1.x, b2.x) && intersect.x <= std::max(b1.x,b2.x) &&
            intersect.y >= std::min(a1.y, a2.y) - .001 && intersect.y <= std::max(a1.y, a2.y) + .001 && //rounding errors lol
            intersect.y >= std::min(b1.y, b2.y) -.001 && intersect.y <= std::max(b1.y, b2.y) + .001;




}
bool lineInVec(const glm::vec2& point1,const glm::vec2& point2, const glm::vec4& r1, double angle) //given points p1 and p2, with p1 having the lesser x value, this draws a line between the 2 points and checks t
{                                                                                                    //see if that line intersects with any of the sides of r1.
    glm::vec2 center = {r1.x + r1.z/2, r1.y + r1.a/2};

    glm::vec2 p1 = rotatePoint(point1,center,-angle);
    glm::vec2 p2 = rotatePoint(point2,center,-angle);

    glm::vec4 points = {std::min(p1.x,p2.x),std::min(p1.y,p2.y),std::max(p1.x,p2.x),std::max(p1.y,p2.y)};

    glm::vec2 topLeft = {r1.x, r1.y};
    glm::vec2 topRight = {r1.x + r1.z, r1.y};
    glm::vec2 botLeft = {r1.x, r1.y + r1.a};
    glm::vec2 botRight = {r1.x + r1.z, r1.y + r1.a};

    return lineInLine(topLeft,topRight,p1,p2) || lineInLine(topRight, botRight, p1, p2) ||
            lineInLine(topLeft, botLeft, p1, p2) || lineInLine(botLeft, botRight, p1, p2);
}

bool vecContains(glm::vec4 r1, glm::vec4 r2)
{
    return (r1.x > r2.x && r1.x + r1.z < r2.x + r2.z && r1.y > r2.y && r1.y + r1.a < r2.y + r2.a);
}

void drawRectangle(RenderProgram& program, const glm::vec3& color, const glm::vec4& rect, double angle)
{
    glm::vec2 center = {rect.x+rect.z/2,rect.y+rect.a/2};
    glm::vec2 topL = rotatePoint({rect.x,rect.y},center,angle);
    glm::vec2 topR = rotatePoint({rect.x+rect.z,rect.y},center,angle);
    glm::vec2 botL = rotatePoint({rect.x,rect.y+rect.a},center,angle);
    glm::vec2 botR = rotatePoint({rect.x+rect.z,rect.y+rect.a},center,angle);
       drawLine(program,color,{{topL.x,topL.y,topR.x,topR.y},
                                {topL.x, topL.y, botL.x, botL.y},
                                {topR.x,topR.y,botR.x,botR.y},
                                {botL.x, botL.y, botR.x, botR.y}});
}
void drawLine(RenderProgram& program, glm::vec3 color, const std::vector<glm::vec4>& points)
{
    int size = points.size();
    GLfloat buffer_data[size*6];/* = {points[0].first.x, points[0].first.y, 0,
    points[0].second.x, points[0].second.y, 0,
    points[1].first.x, points[1].first.y, 0,
    points[1].second.x, points[1].second.y,0};*/
    for (int i = 0;i < size*6; i ++)
    {
        int modI = i/6;
        buffer_data[i] = points[modI].x;
        buffer_data[i+1] = points[modI].y;
        buffer_data[i+2] = 0;
        buffer_data[i+3] = points[modI].z;
        buffer_data[i+4] = points[modI].a;
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
    glDrawArraysInstanced(GL_LINES,0,size*2,size);

}
int loadShaders(const GLchar* source, GLenum shaderType )
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
void RenderProgram::init(int screenWidth, int screenHeight)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE); //set version
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

    glewExperimental = true;

    GLenum err=glewInit();
      if(err!=GLEW_OK) {
        // Problem: glewInit failed, something is seriously wrong.
        std::cout << "glewInit failed: " << glewGetErrorString(err) << std::endl;
      }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    RenderProgram::lineProgram.init("shaders/vertex/simpleVertex.h","shaders/fragment/simpleFragment.h");
    glm::mat4 mat = (glm::ortho(0.0f, (float)screenWidth,(float)screenHeight, 0.0f, -1.0f, 1.0f));
    lineProgram.setMatrix4fv("projection",glm::value_ptr(mat));
    RenderProgram::basicProgram.init("shaders/vertex/vertexShader.h","shaders/fragment/fragmentShader.h");
    basicProgram.setMatrix4fv("projection",glm::value_ptr(mat));
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

Sprite::Sprite( std::string source, bool transparent)
    {
        init(source,transparent);
    }
void Sprite::setTint(glm::vec3 color)
{
    tint = color;
}
void Sprite::init(std::string source, bool transparent)
    {
        load(source, transparent);
    }
void Sprite::loadBuffers() const
{
           //glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        int size = modified.size();
        float verticies[size];
        for (int i = 0; i < size; i ++)
        {
            verticies[i] = modified[i];
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*size, verticies, GL_STATIC_DRAW);
        glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(0);
}
void Sprite::reset()
{
    glBindVertexArray(0);
glBindBuffer(GL_ARRAY_BUFFER,0);
modified.clear();
    for (int i = 0; i < 16; i ++)
    {
        modified.push_back(values[i]);
    }
modIndices.clear();
    for (int i = 0; i < 6; i ++)
    {
        modIndices.push_back(indices[i]);
    }
    tint = {1,1,1};
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
        matrices[i] = glm::translate(matrices[i],{current->rect.x + (current->rect.z*(current->rect.z != 2))/2,current->rect.y + (current->rect.a*(current->rect.a != 2))/2,0}); //scaling messes with the position of the object. If the object is being rendered to a size of 2x2, there is no reason to counteract the scaling.
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
int sizeI = modIndices.size();
int index[sizeI];
for (int i = 0; i < sizeI; i ++)
{
    index[i] = modIndices[i];
}
glDrawElementsInstanced(GL_TRIANGLES,sizeI,GL_UNSIGNED_INT,&index,size);
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

Sprite9::Sprite9(std::string source, bool transparent, glm::vec2 W, glm::vec2 H) : Sprite(source, transparent)
{
    widths = W;
    heights = H;
}
void Sprite9::init(std::string source, bool transparent, glm::vec2 W, glm::vec2 H)
{
    Sprite::init(source, transparent);
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
        bool tooWide = (widths.x+widths.y>rect.z); //whether or not the requested width is bigger than the frame portion;
        bool tooHigh = (heights.x+heights.y>rect.a);

        glm::vec2 modW = {widths.x-(widths.x+widths.y-rect.z)*.5*tooWide,widths.y-(widths.y+widths.x-rect.z)*.5*tooWide};
        glm::vec2 modH = {heights.x-(heights.x+heights.y-rect.z)*.5*tooHigh,heights.y-(heights.y+heights.x-rect.z)*.5*tooHigh};
        //std::cout << modW.x << " " << modW.y << std::endl;
        for ( int g = 0; g < 3; g ++)
        {
            float horiz = (rect.z-modW.x-modW.y)*!tooWide; //the width of the scalable parts. We also have to consider if the width is higher than the frame width
            float vert = (rect.a- modH.x-modH.y)*!tooHigh;//the height of the scalable parts
            for (int h = 0; h < 3; h ++)
            {
                glm::vec4 r = {rect.x + modW.x*(h > 0) + (horiz)*(h>1),rect.y + modH.x*(g>0) + (vert)*(g>1), modW.x*(h == 0) + horiz*(h==1)+modW.y*(h==2), modH.x*(g==0) + vert * (g == 1) + modH.y*(g==2)};
               glm::vec2 center = rotatePoint({r.x+r.z/2,r.y+r.a/2},{rect.x + rect.z/2, rect.y + rect.a/2},current->radians);
               portions.push_back({{center.x - r.z/2,center.y - r.a/2,r.z,r.a},
                                   current->radians,current->tint,{1.0/3*h,(1.0/3)*g,1.0/3,1.0/3}});
            }
        }
    }
    Sprite::renderInstanced(program, portions);
}
