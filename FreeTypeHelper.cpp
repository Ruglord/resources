#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "FreeTypeHelper.h"

RenderProgram Font::wordProgram;

    void Font::init(int screenWidth, int screenHeight)
    {
        wordProgram.init("shaders/vertex/wordVertex.h","shaders/fragment/wordFragment.h");
        wordProgram.setMatrix4fv("projection", glm::value_ptr(glm::ortho(0.0f, (float)screenWidth,0.0f,(float)screenHeight)));
    }
    Font::Font(std::string source, int w,int h)
    {
        init(source,w,h);
    }
    void Font::init(std::string source, int w, int h)
    {


        stanW = w;
        stanH = h;
        font = source;
        FT_Library library;
        if (FT_Init_FreeType(&library))
        {
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        }
        FT_Face face;
        if (FT_New_Face(library, source.c_str(),0,&face))
        {
            std::cout << "Error loading font: " << source << std::endl;
        }
         FT_Set_Pixel_Sizes(face,0,32);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (GLubyte c = 0; c < 128; c++)
        {

            if ( FT_Load_Char(face,c,FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << " " << c << std::endl;
                continue;
            }
            // Generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character;
            character.letter = c;
            character.texture = texture;
            character.size =   glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            character.bearing =   glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
           // character.bearing.x /= 64;
           // character.bearing.y /=64;
             character.advance =    face->glyph->advance.x;
            characters.insert(std::pair<GLchar, Character>(c, character));

        }
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1,&VBO);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
glm::vec2 Font::getDimen(std::string text, GLfloat scale)
{
    std::string::const_iterator c;
    double totalWidth = 0, maxHeight = 0;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = characters[*c];
        GLfloat h = ch.size.y * scale;
        totalWidth += (ch.advance)*scale/64;
        maxHeight += (h-maxHeight)*(h > maxHeight);
    }
    return {totalWidth,maxHeight};
}
glm::vec2 Font::write(RenderProgram& p,std::string text, GLfloat x, GLfloat y, GLfloat scale,glm::vec3 color)
{
    // Activate corresponding render state

  //  p.use();
    //p.setVec3fv("textColor",color);
    p.setVec3fv("textColor",color);
    p.use();
    glBindVertexArray(VAO);
    // Iterate through all characters
    std::string::const_iterator c;
    double originX = x;
    double maxY = y;
    y = stanH - y;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = characters[*c];
        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;
        if (ypos + h > maxY)
        {
            maxY = ypos+h;
        }
            // Update VBO for each character
       GLfloat vertices[6][4] = {
            { xpos,     ypos ,   0.0, 1.0 },
            { xpos + w, ypos,   1.0, 1.0 },
            { xpos,     ypos+h,  0.0, 0.0 },
            { xpos + w, ypos+h,  1.0, 0.0 }
        };

        // Render glyph texture over quad
        // Update content of VBO memory
        //glGenBuffers(1,&VBO);
       // glActiveTexture(GL_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glBindTexture(GL_TEXTURE_2D, ch.texture);


       // glDrawArrays(GL_TRIANGLES, 0, 6);
       glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,&indices);
        glBindBuffer(GL_ARRAY_BUFFER,0);
       // t.renderInstanced(,{{{200,200,64,64}}});
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return glm::vec2(x-originX,maxY);

};

