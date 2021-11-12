#pragma once

#ifndef _STRUCTURES_H
#define _STRUCTURES_H
#include <directxmath.h>
#include <directxcolors.h>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};
#endif