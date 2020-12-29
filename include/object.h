#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// for debug / print
#include <glm/gtx/string_cast.hpp>
#include <stb_image.h>
#include <iostream>
#include <string>
#include <regex>

// static float debugFunc(glm::vec3 L, glm::vec3 A0, glm::vec3 A1, glm::vec3 A2, glm::vec3 extent)
// {
//   float   R0 =  extent.x * (glm::dot(L, A0));
//           R0 += extent.y * (glm::dot(L, A1));
//           R0 += extent.z * (glm::dot(L, A2));
//   return  R0;
// }

typedef struct
{
  glm::vec3 top11;
  glm::vec3 top12;
  glm::vec3 top13;
  glm::vec3 top14;
  glm::vec3 bot11;
  glm::vec3 bot12;
  glm::vec3 bot13;
  glm::vec3 bot14;
} sBoundBox;

typedef struct
{
  glm::vec3 Ax;
  glm::vec3 Ay;
  glm::vec3 Az;
} sBoundBoxEdges;

class Object
{
  public:

    Object(glm::vec3 position, glm::vec3 scale, const char* name, float mass = 1.0f) :
      mVelocity          (glm::vec3(0.0f)),
      mPosition          (position),
      mScale             (scale),
      mInertiaTensor     (glm::mat3(1.0f)), // good for cubes, but must be overwritten for other objects
      mRotationVelocity  (glm::vec3(0.0f, 0.0f, 0.0f)),
      mRotation          (glm::vec3(0.0f, 1.0f, 0.0f)),
      mRotationMag       (0.0f),
      mMass              (mass),
      mDebugOutputOn     (false),
      mIgnoreCollision   (false),
      mName              (name)
    {

    }

  protected:

    glm::vec3       mVelocity;
    glm::vec3       mPosition;
    glm::vec3       mScale;
    glm::mat4       mModel;
    glm::mat3       mInertiaTensor;
    glm::vec3       mRotationVelocity;
    glm::vec3       mRotation;
    float           mRotationMag;
    const float     mMass;
    bool            mDebugOutputOn;
    bool            mIgnoreCollision;
    const char*     mName;
    sBoundBox*      mBoundBoxNonRotated {nullptr}; // not const since we can rescale it!
    sBoundBox*      mBoundBox           {nullptr};
    sBoundBoxEdges* mBoundBoxEdges      {nullptr};

    // utility function for loading a 2D texture from file
    // ---------------------------------------------------
    unsigned int loadTexture(char const * path)
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

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
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

        return textureID;
    }

  public:
    // pure abstracts
    virtual void draw() = 0;
    virtual void drawInit() = 0;
    virtual float containingRadius() = 0;

    void checkForErrors()
    {
      static const char* offending = "The offending command is ignored and has no other side effect than to set the error flag.";
      GLenum err;
      while((err = glGetError()) != GL_NO_ERROR)
      {
        std::cout << "Error found for " << mName << std::endl;
        switch (err)
        {
          // case GL_NO_ERROR:
          //   std::cout << "GL_NO_ERROR - No error has been recorded. The value of this symbolic constant is guaranteed to be 0." << std::endl;
          //   break;            
          case GL_INVALID_ENUM:
            std::cout << "GL_INVALID_ENUM - An unacceptable value is specified for an enumerated argument. " << offending << std::endl;
            break;            
          case GL_INVALID_VALUE:
            std::cout << "GL_INVALID_VALUE - A numeric argument is out of range. " << offending << std::endl;
            break;            
          case GL_INVALID_OPERATION:
            std::cout << "GL_INVALID_OPERATION - The specified operation is not allowed in the current state. " << offending << std::endl;
            break;            
          case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION - The framebuffer object is not complete. " << offending << std::endl;
            break;            
          case GL_OUT_OF_MEMORY:
            std::cout << "GL_OUT_OF_MEMORY - There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded." << std::endl;
            break;            
          // case GL_STACK_UNDERFLOW:
          //   std::cout << "GL_STACK_UNDERFLOW - An attempt has been made to perform an operation that would cause an internal stack to underflow." << std::endl;
          //   break;            
          // case GL_STACK_OVERFLOW:
          //   std::cout << "GL_STACK_OVERFLOW - An attempt has been made to perform an operation that would cause an internal stack to overflow.." << std::endl;
          //   break;
          default:
            std::cout << "Error (" << err << ") unknown." << std::endl;
        }
      }
    }

    bool ignoreColl()
    {
      return mIgnoreCollision;
    }

    void printObject()
    {
      std::string modelFormatted {glm::to_string(this->mModel)};
      const std::regex  re {R"(\)\,\s\()"};
      const std::string replStr = ")\n(";
      const std::regex  reStart {R"(\(\()"};
      const std::string replStrStart = "\n\(\(";
      modelFormatted = std::regex_replace(modelFormatted, re,      replStr);
      modelFormatted = std::regex_replace(modelFormatted, reStart, replStrStart);

      std::string boundBoxStr = "\n";
      if (mBoundBox)
      {
        boundBoxStr += "top11: " + glm::to_string(mBoundBox->top11) + "\n";
        boundBoxStr += "top12: " + glm::to_string(mBoundBox->top12) + "\n";
        boundBoxStr += "top13: " + glm::to_string(mBoundBox->top13) + "\n";
        boundBoxStr += "top14: " + glm::to_string(mBoundBox->top14) + "\n";
        boundBoxStr += "bot11: " + glm::to_string(mBoundBox->bot11) + "\n";
        boundBoxStr += "bot12: " + glm::to_string(mBoundBox->bot12) + "\n";
        boundBoxStr += "bot13: " + glm::to_string(mBoundBox->bot13) + "\n";
        boundBoxStr += "bot14: " + glm::to_string(mBoundBox->bot14) + "\n";
      }
      if (mBoundBoxEdges)
      {
        boundBoxStr += "Ax: " + glm::to_string(mBoundBoxEdges->Ax) + "\n";
        boundBoxStr += "Ay: " + glm::to_string(mBoundBoxEdges->Ay) + "\n";
        boundBoxStr += "Az: " + glm::to_string(mBoundBoxEdges->Az) + "\n";
      }

      std::cout << "name: "             <<  this->mName                             << "\n"
                << "position: "         <<  glm::to_string(this->mPosition)         << "\n"
                << "scale: "            <<  glm::to_string(this->mScale)            << "\n"
                << "velocity: "         <<  glm::to_string(this->mVelocity)         << "\n"
                << "rotation: "         <<  glm::to_string(this->mRotation)         << "\n"
                << "rotationVelocity: " <<  glm::to_string(this->mRotationVelocity) << "\n"
                << "mass: "             <<  this->mMass                             << "\n"
                << "model: "            <<  modelFormatted                          << "\n"
                << "debugOutputOn: "    <<  this->mDebugOutputOn                    << "\n"
                << "boundbox: "         <<  boundBoxStr                             << "\n"
                << std::endl;
    }

    void updateModel()
    {
      mModel = glm::mat4(1.0f);
      mModel = glm::translate(mModel, mPosition);
      mModel = glm::scale(mModel, mScale);
      if (mRotationMag) // check that we do not spin around 0 length vector
      {
        mModel = glm::rotate(mModel, mRotationMag, glm::normalize(mRotation));
      }
      else
      {
        // mModel = glm::rotate(mModel, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
      }
      
    }

    void updatePosition(float dt)
    {
      mPosition += mVelocity * dt;
      if (this->mDebugOutputOn)
      {
        std::cout << glm::to_string(mPosition) << std::endl;
      }
      this->updateModel();
    }

    void updateRotation(float dt)
    {
      mRotation += mRotationVelocity * dt;
      this->updateModel();
    }

    void setVelocity(const glm::vec3& velocity)
    {
      this->mVelocity = velocity;
    }

    // sets a fixed rotation, does not set or rotation velocity
    // rotation is a vector about which to rotate, and the magnitue the 
    // radians to rotate. Set as 0 length vector to set as non-rotated.
    void setRotation(const glm::vec3& rotation, float rotMag)
    {
      this->mRotation     = rotation;
      this->mRotationMag  = rotMag; //glm::length(this->mRotation);
    }

    void setScale(const glm::vec3& scale)
    {
      this->mScale = scale;
    }

    void setRotationVelocity(const glm::vec3& rotationVelocity)
    {
      this->mRotationVelocity = rotationVelocity;
    }

    glm::vec3 getPosition()
    {
      return this->mPosition;
    }

    glm::vec3 getVelocity()
    {
      return this->mVelocity;
    }

    void setDebugInfo(bool on)
    {

    }

    /* 
      notes on coordinate system
      The camera points along the z-axis (moving forward in the beginning is going to NEGATIVE z).
      This means that the horizontal plane is the xz-plane. Looking from "below" we see the following:

                  (top14)____________(top13)
      z+       y+    /               /|
      ^        ^    /               / |
      |       /    /               /  |
      |      /(bot14)__________(bot13)|
      |     /    |         c0     |   /
      |    /     |          |_____|_ /
      |   /      |         /      | /
      |  /       |        /       |/
      | /     (bot11)____/_____(bot12)
      |/
      +- - - - - - - - - - - - - - - - - -> x+

      from the top it looks like this (notice the negative z axis).
      so the convention is that:
      top/bot11 is the lowest z and lowest x. in this way the bot / top xx is mirrored for all xx.
      note that this is before any rotation of the coordinates!
      
      z-
      ^
      |
      |      (top11) -------- (top12)
      |          |               |
      |          |               |
      |          |               |
      |          |               |
      |      (top14) -------- (top13)
      |
      +- - - - - - - - - - - - - - - - - - > x+
    
      the collision will use the algorithm (and naming!) described here:
      https://www.geometrictools.com/Documentation/DynamicCollisionDetection.pdf [DCD]
      it is also in the docs/ folder (at the project root dir). Doc will be referenced below.
    */
    bool checkCollision(Object* obj)
    {
      Object* A = this; // alias to help with the naming
      Object* B = obj;  // alias to help with the naming
      
      // skip test if spheres not overlapping
      float minR = A->containingRadius() + B->containingRadius();
      minR *= minR; // square
      glm::vec3 C0 = A->mPosition; // center of A
      glm::vec3 C1 = B->mPosition; // center of B
      glm::vec3 D  = C1 - C0; // [DCD p. 6]
      float actualR = glm::dot(D, D); // by convention we do not divide by 4 in these calcs to speed up computation
      std::cout << "ABdist = " << glm::to_string(D) << " len = " << actualR << " minR = " << minR << std::endl;
      if (actualR > minR) 
      {
        std::cout << "Skipping test!" << std::endl;
        return false;
      }

      const size_t noOfTests = 15;
      // check that BoundBox is not null here
      A->UpdateBoundBox();
      B->UpdateBoundBox();
      
      // generate the 15 axis [DCD Table 1, p. 7]
      // OPTIMIZE: later on, start with the easy ones (non-cross products)
      // and then generate one at a time and check - no reason to calculate if
      // one before fails
      glm::vec3 L[15];
      int i = 0;
      L[i++] = A->mBoundBoxEdges->Ax;
      L[i++] = A->mBoundBoxEdges->Ay;
      L[i++] = A->mBoundBoxEdges->Az;
      L[i++] = B->mBoundBoxEdges->Ax;
      L[i++] = B->mBoundBoxEdges->Ay;
      L[i++] = B->mBoundBoxEdges->Az;
      const size_t noOfDims = 3; // also offset in L[] to get to the corresponding axis in the B object
      for (size_t n = 0; n < noOfDims; n++)
      {
        for (size_t m = 0; m < noOfDims; m++)
        {
          L[i++] = glm::cross(L[n], L[m + noOfDims]);
        }
      }

      // calculate c_ij from orthonormal edges [DCD p. 6]
      // OPTIMIZE: by only calculating c_ij as needed with A_i.dot(B_j)  [DCD p. 6]
      //  this can done by pre-allocating an array of floats (doubles?) and fill in as needed
      glm::mat3 AEdges = glm::mat3(L[0], L[1], L[2]);
      glm::mat3 BEdges = glm::mat3(L[3], L[4], L[5]);
      // matrix access is [col][row]
      glm::mat3 C_ij = glm::transpose(AEdges) * BEdges;
      glm::mat3 C_ij_abs(1.0f);

      // OPTIMIZE: this should be done on the dot product instead!
      for (size_t n = 0; n < noOfDims; n++)
      {
        C_ij_abs[n][0] = glm::abs(C_ij[n][0]);
        C_ij_abs[n][1] = glm::abs(C_ij[n][1]);
        C_ij_abs[n][2] = glm::abs(C_ij[n][2]);
      }

      // a_i and b_i is the length of the edges
      glm::vec3 a_i = 0.5f * A->mScale; // extent in [DCD Table 1, p. 5] is only half of the side length
      glm::vec3 b_i = 0.5f * B->mScale;
      float R0[noOfTests];
      float R1[noOfTests];
      float R[noOfTests];

      // note that col / row order is switched for glm lib compared to ref [DCD].
      R0[0]  = a_i.x;
      R0[1]  = a_i.y;
      R0[2]  = a_i.z;
      R0[3]  = a_i.x * C_ij_abs[0][0] + a_i.y * C_ij_abs[0][1] + a_i.z * C_ij_abs[0][2];
      R0[4]  = a_i.x * C_ij_abs[1][0] + a_i.y * C_ij_abs[1][1] + a_i.z * C_ij_abs[1][2];
      R0[5]  = a_i.x * C_ij_abs[2][0] + a_i.y * C_ij_abs[2][1] + a_i.z * C_ij_abs[2][2];
      R0[6]  = a_i.y * C_ij_abs[0][2] + a_i.z * C_ij_abs[0][1];
      R0[7]  = a_i.y * C_ij_abs[1][2] + a_i.z * C_ij_abs[1][1];
      R0[8]  = a_i.y * C_ij_abs[2][2] + a_i.z * C_ij_abs[2][1];
      R0[9]  = a_i.x * C_ij_abs[0][2] + a_i.z * C_ij_abs[0][0];
      R0[10] = a_i.x * C_ij_abs[1][2] + a_i.z * C_ij_abs[1][0];
      R0[11] = a_i.x * C_ij_abs[2][2] + a_i.z * C_ij_abs[2][0];
      R0[12] = a_i.x * C_ij_abs[0][1] + a_i.y * C_ij_abs[0][0];
      R0[13] = a_i.x * C_ij_abs[1][1] + a_i.y * C_ij_abs[1][0];
      R0[14] = a_i.x * C_ij_abs[2][1] + a_i.y * C_ij_abs[2][0];


      R1[0]  = b_i.x * C_ij_abs[0][0] + b_i.y * C_ij_abs[1][0] + b_i.z * C_ij_abs[2][0];
      R1[1]  = b_i.x * C_ij_abs[0][1] + b_i.y * C_ij_abs[1][1] + b_i.z * C_ij_abs[2][1];
      R1[2]  = b_i.x * C_ij_abs[0][2] + b_i.y * C_ij_abs[1][2] + b_i.z * C_ij_abs[2][2];
      R1[3]  = b_i.x;
      R1[4]  = b_i.y;
      R1[5]  = b_i.z;
      R1[6]  = b_i.y * C_ij_abs[2][0] + b_i.z * C_ij_abs[1][0];
      R1[7]  = b_i.x * C_ij_abs[2][0] + b_i.z * C_ij_abs[0][0];
      R1[8]  = b_i.x * C_ij_abs[1][0] + b_i.y * C_ij_abs[0][0];
      R1[9]  = b_i.y * C_ij_abs[2][1] + b_i.z * C_ij_abs[1][1];
      R1[10] = b_i.x * C_ij_abs[2][1] + b_i.z * C_ij_abs[0][1];
      R1[11] = b_i.x * C_ij_abs[1][1] + b_i.y * C_ij_abs[0][1];
      R1[12] = b_i.y * C_ij_abs[2][2] + b_i.z * C_ij_abs[1][2];
      R1[13] = b_i.x * C_ij_abs[2][2] + b_i.z * C_ij_abs[0][2];
      R1[14] = b_i.x * C_ij_abs[1][2] + b_i.y * C_ij_abs[0][2];

      // OPTIMIZE:
      // all the dot product can be calculated separately and be reused in later calculations
      // then the abs can be taken when needed
      R[0]   = glm::abs(glm::dot(L[0], D));
      R[1]   = glm::abs(glm::dot(L[1], D));
      R[2]   = glm::abs(glm::dot(L[2], D));
      R[3]   = glm::abs(glm::dot(L[3], D));
      R[4]   = glm::abs(glm::dot(L[4], D));
      R[5]   = glm::abs(glm::dot(L[5], D));
      R[6]   = glm::abs((C_ij[0][1] * glm::dot(L[2], D)) - (C_ij[0][2] * glm::dot(L[1], D)));
      R[7]   = glm::abs((C_ij[1][1] * glm::dot(L[2], D)) - (C_ij[1][2] * glm::dot(L[1], D)));
      R[8]   = glm::abs((C_ij[2][1] * glm::dot(L[2], D)) - (C_ij[2][2] * glm::dot(L[1], D)));
      R[9]   = glm::abs((C_ij[0][2] * glm::dot(L[0], D)) - (C_ij[0][0] * glm::dot(L[2], D)));
      R[10]  = glm::abs((C_ij[1][2] * glm::dot(L[0], D)) - (C_ij[1][0] * glm::dot(L[2], D)));
      R[11]  = glm::abs((C_ij[2][2] * glm::dot(L[0], D)) - (C_ij[2][0] * glm::dot(L[2], D)));
      R[12]  = glm::abs((C_ij[0][0] * glm::dot(L[1], D)) - (C_ij[0][1] * glm::dot(L[0], D)));
      R[13]  = glm::abs((C_ij[1][0] * glm::dot(L[1], D)) - (C_ij[1][1] * glm::dot(L[0], D)));
      R[14]  = glm::abs((C_ij[2][0] * glm::dot(L[1], D)) - (C_ij[2][1] * glm::dot(L[0], D)));

      // OPTIMIZE: if any of the unit vectors of the 2 objects are aligned
      //           we do not need to check those axes again!
      for (size_t n = 0; n < noOfTests; n++)
      {
        // std::cout << "n:  " << n     << "\n"
        //           << "R:  " << R[n]  << "\n"
        //           << "R0: " << R0[n] << "\n"
        //           << "R1: " << R1[n] << std::endl;

        if (R[n] > (R0[n] + R1[n]))
        {
          // debug output
          std::cout.precision(3);
          std::cout << "Testing for " << A->mName << "(=A) vs. " << B->mName << "(=B)" << std::endl;
          std::cout <<  "R0\tR1\tR\tR0+R1: n=" << n << "\n" 
                    <<  R0[n]  << "\t" << R1[n]  << "\t" << R[n] << "\t" << R0[n] + R1[n] << "\n"
                    <<  "C0 "  << glm::to_string(C0) << "\n"
                    <<  "C1 "  << glm::to_string(C1) << "\n"
                    <<  "D "   << glm::to_string(D)  << "\n"
                    <<  "a_i "   << glm::to_string(a_i)  << "\n"
                    <<  "b_i "   << glm::to_string(b_i)  << "\n"
                    <<  "C_ij "   << glm::to_string(C_ij)  << "\n"
                    // <<  "Alt R0: " << debugFunc(L[n], L[0], L[1], L[2], a_i) << "\n"
                    <<  "Projected axis: " << glm::to_string(L[n]) << "\n" << std::endl;
          return false;
        }
      }
      std::cout << "Collision!!!!" << std::endl;

      
      return true;
    }


    // this only works for non rotated boxes
    // bool checkCollision(Object* obj)
    // {
    //   // WATCH OUT HERE -> WHAT HAPPENS WHEN THE BOX ROTATES?
    //   glm::vec3 a_botLeft(0.0f);  // min val in cartesian system
    //   glm::vec3 a_topRight(0.0f); // max val in cartesian system
    //   glm::vec3 b_botLeft(0.0f);  // min val in cartesian system
    //   glm::vec3 b_topRight(0.0f); // max val in cartesian system
    //   // check that BoundBox is not null here
    //   this->BoundBox(a_botLeft, a_topRight);
    //   obj->BoundBox(b_botLeft, b_topRight);
    //   // std::cout << "a_botLeft: " <<  glm::to_string(a_botLeft)
    //   //           << "a_topRight: " <<  glm::to_string(a_topRight)
    //   //           << "b_botLeft: " <<  glm::to_string(b_botLeft)
    //   //           << "b_topRight: " <<  glm::to_string(b_topRight)
    //   //           << std::endl;
    //   return ((a_botLeft.x <= b_topRight.x && a_topRight.x >= b_botLeft.x) &&
    //           (a_botLeft.y <= b_topRight.y && a_topRight.y >= b_botLeft.y) &&
    //           (a_botLeft.z <= b_topRight.z && a_topRight.z >= b_botLeft.z));
    // }

    void calcCollision(Object* obj)
    {
      double e = 1.0; // coefficient of restitution. Max 1 (perfect elastic). Min 0 (all kinetic energi lost as heat or deformation) 
      // see article 4 and https://en.wikipedia.org/wiki/Collision_response#Impulse-based_contact_model
      // algo:
      // calculate v_p1 and v_p2 AT THE POINT OF CONTACT, so if they are rotating we have:
      // v_pi = v_i + w_i x r_i  (x is crossproduct) for i = 1 and 2
      
      // CoM (i.e., linear) velocities
      glm::vec3 v_p1 = this->getVelocity();
      glm::vec3 v_p2 = obj->getVelocity();
      // get masses
      float m1 = this->mMass;
      float m2 = obj->mMass;

      // calculate relative velocity vr = v_p1 - v_p2
      // REMEMBER TO INCLUDE ROTATION VELOCITY LATER ON!
      glm::vec3 vr = v_p2 - v_p1;
      // FOR NOW ONLY! calculate the normal. This should be done in a better way
      // glm::vec3 n = glm::normalize(this->getVelocity());
      glm::vec3 n = glm::normalize(this->mPosition - obj->mPosition);
      
      // calculate the impulse magnitude nominator
      double jr_nom = -(1 + e) * (glm::dot(vr, n)); // left out for now: dot with normal vector
      // calculate the impulse magnitude denominator
      double jr_denom = 1 / m1 + 1 / m2;
      double jr = jr_nom / jr_denom;
      glm::vec3 vjr = ((float)jr) * n;

      // calculate new velocities
      glm::vec3 v_p1a = v_p1 - vjr / m1;
      glm::vec3 v_p2a = v_p2 + vjr / m2;

      std::cout << "vr "  << glm::to_string(vr)  << std::endl;
      std::cout << "n "   << glm::to_string(n)   << std::endl;
      std::cout << "jr "  << jr                  << std::endl;
      std::cout << "vjr " << glm::to_string(vjr) << std::endl;
      
      std::cout << "Before " << std::endl;
      this->printObject();
      obj->printObject();

      // update linear velocities
      this->setVelocity(v_p1a);
      obj->setVelocity(v_p2a);

      std::cout << "After " << std::endl;
      this->printObject();
      obj->printObject();

      // we should remember where the objects were in the previous frame and move them back there
      // as a simple first approximation
      std::cout << "Moving object a bit away from each other! This should be handled in another way\n" << std::endl;
      this->updatePosition(0.03);
      obj->updatePosition(0.03);
    }

    private:
      virtual void UpdateBoundBox() = 0;
};
