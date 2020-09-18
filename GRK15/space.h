#ifndef SPACE_H
#define SPACE_H

#include "indexmodel.h"
#include "movablemodel.h"
#include "texturemodel.h"
#include "matma.h"
#include "texturecameraprogram.h"

class Space : public IndexModel, public MovableModel, public TextureModel {
public:
    void Initialize(int m, int n);
    void Draw(const TextureCameraProgram& program) const;
private:
    int n_; // mesh parameter
    int m_; // mesh parameter

};

#endif // SPACE_H
