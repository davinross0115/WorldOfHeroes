#ifndef VEC3_H
#define VEC3_H

#include "vec2.h"

union vec3
{
	struct
	{
		r32 x, y, z;
	};
	struct
	{
		r32 r, g, b;
	};
	struct
	{
		vec2 xy;
		r32 ignored0;
	};
	struct
	{
		r32 ignored1;
		vec2 yz;
	};
	r32 E[3];
};

internal_ vec3 operator*(r32 a, vec3 b)
{
	vec3 result = {};
	result.x = a * b.x;
	result.y = a * b.y;
	result.z = a * b.z;
	return result;
}

internal_ vec3 operator*(vec3 b, r32 a)
{
	vec3 result = a*b;
	return result;
}

internal_ vec3& operator*=(vec3& b, r32 a)
{
	b = a * b;
	return b;
}

internal_ vec3 operator-(vec3 a)
{
	vec3 result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	return result;
}

internal_ vec3 operator-(vec3 a, vec3 b)
{
	vec3 result;
	result.x = a.x - b.y;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

internal_ vec3 operator+(vec3 a, vec3 b)
{
	vec3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

internal_ vec3& operator+=(vec3& a, vec3 b)
{
	a = a + b;
	return a;
}

internal_ vec3 Hadamard(vec3 a, vec3 b)
{
	vec3 result = { a.x * b.x, a.y * b.y, a.z * b.z };
	return result;
}

internal_ r32 Dot(vec3 a, vec3 b)
{
	r32 result = a.x*b.x + a.y*b.y + a.z*b.z;
	return result;
}

internal_ r32 LengthSq(vec3 a)
{
	r32 result = Dot(a, a);
	return result;
}

internal_ r32 Length(vec3 a)
{
	r32 result = (r32)sqrt(LengthSq(a));
	return result;
}

internal_ vec3 Normalize(vec3 A)
{
	vec3 Result = A * (1.0f / Length(A));

	return Result;
}

internal_ vec3 Cross(vec3 A, vec3 B)
{
	vec3 C = { 0 };
	C.x = (A.y * B.z) - (A.z * B.y);
	C.y = (A.z * B.x) - (A.x * B.z);
	C.z = (A.x * B.y) - (A.y * B.x);

	return C;
}

internal_ vec3 Clamp01(vec3 val)
{
	vec3 result;
	result.x = Clamp01(val.x);
	result.y = Clamp01(val.y);
	result.z = Clamp01(val.z);
	return result;
}

internal_ vec3 Vec3(r32 x, r32 y, r32 z)
{
	vec3 result = {};
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}

internal_ vec3 Vec3(vec2 xy, r32 z)
{
	vec3 result = {};
	result.xy = xy;
	result.z = z;
	return result;
}

internal_ vec3 Vec3(r32 x, vec2 yz)
{
	vec3 result = {};
	result.x = x;
	result.yz = yz;
	return result;
}

internal_ vec3 Vec3(r32 x)
{
	vec3 Result = {};

	Result.x = x;
	Result.y = x;
	Result.z = x;

	return Result;
}

// Mat4x4

struct mat4x4
{
	r32 E[4][4];
};

internal_ inline mat4x4
operator*(mat4x4 A, mat4x4 B)
{
	// TODO: Optimize this piece of code to make it better
	mat4x4 R = {};

	for (int r = 0; r <= 3; ++r)
	{
		for (int c = 0; c <= 3; ++c)
		{
			for (int i = 0; i <= 3; ++i)
			{
				R.E[r][c] += A.E[r][i] * B.E[i][c];
			}
		}
	}

	return R;
}

internal_ vec3
Transform(mat4x4 A, vec3 P, r32 PW = 1.0f)
{
	vec3 R = {};

	R.x = P.x*A.E[0][0] + P.y*A.E[0][1] + P.z*A.E[0][2] + PW*A.E[0][3];
	R.y = P.x*A.E[1][0] + P.y*A.E[1][1] + P.z*A.E[1][2] + PW*A.E[1][3];
	R.z = P.x*A.E[2][0] + P.y*A.E[2][1] + P.z*A.E[2][2] + PW*A.E[2][3];

	return R;
}

internal_ inline vec3
operator*(mat4x4 A, vec3 P)
{
	vec3 R = Transform(A, P, 1.0f);

	return R;
}

internal_ inline vec3
operator*(vec3 P, mat4x4 A)
{
	vec3 R = Transform(A, P, 1.0f);

	return R;
}

internal_ mat4x4
Identity(void)
{
	mat4x4 R =
	{
		{
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 }
		},
	};

	return R;
}

internal_ mat4x4
RotateX(r32 Angle)
{
	r32 c = cosf(Angle);
	r32 s = sinf(Angle);

	mat4x4 R =
	{
		{ { 1, 0, 0, 0 },
		{ 0, c,-s, 0 },
		{ 0, s, c, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

internal_ mat4x4
RotateY(r32 Angle)
{
	r32 c = cosf(Angle);
	r32 s = sinf(Angle);

	mat4x4 R =
	{
		{ { c, 0, s, 0 },
		{ 0, 1, 0, 0 },
		{ -s, 0, c, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

internal_ mat4x4
RotateZ(r32 Angle)
{
	r32 c = cosf(Angle);
	r32 s = sinf(Angle);

	mat4x4 R =
	{
		{ { c,-s, 0, 0 },
		{ s, c, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

internal_ mat4x4
Transpose(mat4x4 A)
{
	mat4x4 R = {};

	for (int j = 0; j <= 3; j++)
	{
		for (int i = 0; i <= 3; i++)
		{
			R.E[j][i] = A.E[i][j];
		}
	}

	return R;
}

internal_ mat4x4
PerspectiveProjection(r32 AspectWidthOverHeight, r32 FocalLength, r32 NearZ, r32 FarZ)
{
	r32 a = 1.0f;
	r32 b = AspectWidthOverHeight;
	r32 c = FocalLength;

	r32 n = NearZ; // NOTE: Near Clip plane _distance_
	r32 f = FarZ; // NOTE: Far Clip plane _distance_

#if 1
				  // NOTE: These are the perspective correct terms, for when you divide by -z
	r32 d = (n + f) / (n - f);
	r32 e = (2 * f*n) / (n - f);
#else
				  // NOTE: These are the non-perspective corrected terms, for orthographic
	r32 d = 2.0f / (n - f);
	r32 e = (n + f) / (n - f);
#endif

	mat4x4 R =
	{
		{ { a*c,   0,  0,  0 },
		{ 0, b*c,  0,  0 },
		{ 0,   0,  d,  e },
		{ 0,   0, -1,  0 } },
	};

	return R;
}

internal_ mat4x4
OrthographicProjection(r32 AspectWidthOverHeight)
{
	r32 a = 1.0f;
	r32 b = AspectWidthOverHeight;

	mat4x4 R =
	{
		{ { a, 0, 0, 0 },
		{ 0, b, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 } },
	};

	return R;
}

internal_ mat4x4
Column3x3(vec3 X, vec3 Y, vec3 Z)
{
	mat4x4 R =
	{
		{ { X.x, Y.x, Z.x, 0 },
		{ X.y, Y.y, Z.y, 0 },
		{ X.z, Y.z, Z.z, 0 },
		{ 0,   0,   0, 1 } },
	};

	return R;
}

internal_ mat4x4
Row3x3(vec3 X, vec3 Y, vec3 Z)
{
	mat4x4 R =
	{
		{ { X.x, X.y, X.z, 0 },
		{ Y.x, Y.y, Y.z, 0 },
		{ Z.x, Z.y, Z.z, 0 },
		{ 0,   0,   0, 1 } },
	};

	return R;
}

internal_ mat4x4
Translate(mat4x4 A, vec3 T)
{
	mat4x4 R = A;
	R.E[0][3] += T.x;
	R.E[1][3] += T.y;
	R.E[2][3] += T.z;

	return R;
}

internal_ inline vec3
GetColumn(mat4x4 A, u32 C)
{
	vec3 R = { A.E[0][C], A.E[1][C], A.E[2][C] };

	return R;
}

internal_ inline vec3
GetRow(mat4x4 A, u32 R)
{
	vec3 Result = { A.E[R][0], A.E[R][1], A.E[R][2] };

	return Result;
}

internal_ mat4x4
CameraTransform(vec3 X, vec3 Y, vec3 Z, vec3 P)
{
	mat4x4 R = Row3x3(X, Y, Z);
	R = Translate(R, -(R*P));

	return R;
}

internal_ mat4x4
LookAt(vec3 eye, vec3 target, vec3 up)
{
	vec3 zAxis = Normalize(eye - target);
	vec3 xAxis = Normalize(Cross(up, zAxis));
	vec3 yAxis = Normalize(Cross(zAxis, xAxis));

	r32 eyeX = -Dot(xAxis, eye);
	r32 eyeY = -Dot(yAxis, eye);
	r32 eyeZ = -Dot(zAxis, eye);

	mat4x4 viewMatrix =
	{
		xAxis.x,	yAxis.x,	zAxis.x,	0.0f,
		xAxis.y,	yAxis.y,	zAxis.y,	0.0f,
		xAxis.z,	yAxis.z,	zAxis.z,	0.0f,
		eyeX,		eyeY,		eyeZ,		1.0f,
	};

	return viewMatrix;
}

#endif