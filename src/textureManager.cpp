#include "../include/textureManager.h"
#include <glad/glad.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// for debug / print
// #include <glm/gtx/string_cast.hpp>
#include <stb_image.h>
#include <iostream>
#include <cassert>
// #include <string>
// #include <regex>
// #include <vector>

static const size_t   maxNoOfTextures = 32; // TODO: poll OpenGL instead of setting fixed here
static bool           slotUsed[maxNoOfTextures] = {false};

TextureManager::TextureManager()
{}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
static TextureManager::TextureId loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return static_cast<TextureManager::TextureId>(textureID);
}

const char* TextureManager::GetTexturePath(EnumTexture texture)
{
  switch (texture)
  {
    case kInvalid:      return (const char*)"";
    case kDan:          return (const char*)"../textures/dan_bono.jpg";
    case kMetal:        return (const char*)"../textures/metal.png";
    case kRedWindow:    return (const char*)"../textures/window.png";
    default:      
        std::cout << "INVALID TEXTURE IN \n" << __FUNCTION__; 
        exit(0);
        return "";
  }
}

size_t TextureManager::GetAvailableSlot(bool& noSlotAvailable)
{
    for (size_t i = 0; i < maxNoOfTextures; i++)
    {
        if (!slotUsed[i])
        {
            slotUsed[i] = true;
            noSlotAvailable = false;
            return i;
        }
    }
    noSlotAvailable = true;
    return 0;
}

bool TextureManager::DeregisterTexture(EnumTexture texture)
{
    auto texturePair = mGeneratedTextures.find(texture);
    if (texturePair == mGeneratedTextures.end())
    {
        return false;
    }
    
    if (texturePair->second.userCnt > 0)
    {
        // decrease number of users
        texturePair->second.userCnt--;
        if (texturePair->second.userCnt == 0)
        {
            slotUsed[texturePair->second.textureIndex] = false;
            // ALSO, DELETE TEXTURE HERE with glDeleteTextures
        }
    }
    return true;
}

bool TextureManager::RegisterTexture(EnumTexture texture)
{
    // check if we already have texture
    auto texturePair = mGeneratedTextures.find(texture);
    if (texturePair != mGeneratedTextures.end())
    {
        // we already loaded the texture - increment user cnt
        texturePair->second.userCnt++;
        return true;
    }
    // we need a new slot
    bool noSlotAvailable;
    size_t slotIndex = GetAvailableSlot(noSlotAvailable);
    if (noSlotAvailable)
    {
        return false;
    }

    sTexture textureStruct;
    textureStruct.textureIndex = slotIndex;

    // uncomment and use the SHADER to point to the needed texture slot!
    // we probably dont even need to rebind in every loop!
    // glActiveTexture(GL_TEXTURE0 + slotIndex); // activate index
    glActiveTexture(GL_TEXTURE0);
    textureStruct.id = loadTexture(GetTexturePath(texture));
    mGeneratedTextures.insert(std::make_pair(texture, textureStruct));
    glActiveTexture(GL_TEXTURE0); // back to default
    return true;
}

bool TextureManager::ActivateTexture(EnumTexture texture)
{
    assert(texture != kInvalid);
    auto texturePair = mGeneratedTextures.find(texture);
    if (texturePair == mGeneratedTextures.end())
    {
        // texture not found
        return false;
    }
    if (texture == mBoundTexture)
    {
        return true;
    }
    mBoundTexture = texture;
    
    // glActiveTexture(GL_TEXTURE0 + texturePair->second.textureIndex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturePair->second.id);
    glActiveTexture(GL_TEXTURE0); // back to default
    return true;
}
