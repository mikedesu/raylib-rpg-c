#pragma once

#define SIZEOFBOOL(n) (sizeof(bool) * n)

#define SIZEOFVEC3(n) (sizeof(Vector3) * n)

#define SIZEOFINT(n) (sizeof(int) * n)

#define SIZEOFFLOAT(n) (sizeof(float) * n)

#define SIZEOFT2D(n) (sizeof(Texture2D) * n)

#define SIZEOFCAMERAS (sizeof(Camera2D) + sizeof(Camera3D))

#define SIZEOFCOLOR(n) (sizeof(Color) * n)
