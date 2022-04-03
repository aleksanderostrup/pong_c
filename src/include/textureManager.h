#pragma once

#include <unordered_map>
#include <stdint.h>

/* 
    TextureManager manages textures globally for one GPU.
    Each user should register a texture and gets a corresponding TextureId
    in return, used to set that texture.
 */
class TextureManager
{
  public:

    using TextureId = uint32_t; // USE GLuint ?
    enum class Texture
    {
      kInvalid      = 0,
      kDan          = 1,
      kMetal        = 2,
      kRedWindow    = 3,
    };

    TextureManager();
    // ~TextureManager();

    bool    RegisterTexture           (Texture texture);
    bool    DeregisterTexture         (Texture texture);
    bool    ActivateTexture           (Texture texture);
    
    // size_t  GetMaxSimultaneousTextures();

  private:
    struct sTexture {
        TextureId id;
        uint32_t  textureIndex;
        size_t    userCnt; // keep score of how many are using this texture
    };

    std::unordered_map<Texture, sTexture> mGeneratedTextures;
    Texture                               mBoundTexture = Texture::kInvalid;

    const char* GetTexturePath(Texture texture);
    size_t  GetAvailableSlot  (bool& noSlotAvailable);
};
