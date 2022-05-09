#include "objects/spritebatch/spritebatch.h"

using namespace love;

SpriteBatch::SpriteBatch(Graphics* graphics, love::Texture* texture, int size) :
    common::SpriteBatch(graphics, texture, size)
{}

SpriteBatch::~SpriteBatch()
{}

void SpriteBatch::Flush()
{}

int SpriteBatch::Add(Quad* quad, const Matrix4& matrix, int index)
{}

void SpriteBatch::Draw(Graphics* graphics, const Matrix4& matrix)
{}

void SpriteBatch::SetBufferSize(int newSize)
{}
