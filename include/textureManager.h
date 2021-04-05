#pragma once

#include <unordered_map>

/* 
    TextureManager manages textures globally for one GPU.
    Each user should register a texture and gets a corresponding TextureId
    in return, used to set that texture.
 */
class TextureManager
{
  public:

    typedef uint32_t TextureId; // USE GLuint ?
    enum EnumTexture
    {
      kInvalid      = 0,
      kDan          = 1,
      kMetal        = 2,
      kRedWindow    = 3,
    };

    TextureManager();
    // ~TextureManager();

    bool    RegisterTexture           (EnumTexture texture);
    bool    DeregisterTexture         (EnumTexture texture);
    bool    ActivateTexture           (EnumTexture texture);
    
    // size_t  GetMaxSimultaneousTextures();

  private:
    struct sTexture {
        TextureId id;
        uint32_t  textureIndex;
        size_t    userCnt; // keep score of how many are using this texture
    };

    std::unordered_map<EnumTexture, sTexture> mGeneratedTextures;
    EnumTexture                               mBoundTexture = kInvalid;

    const char* GetTexturePath(EnumTexture texture);
    size_t  GetAvailableSlot  (bool& noSlotAvailable);
};
