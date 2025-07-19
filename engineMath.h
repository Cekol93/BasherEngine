#pragma once
#ifndef ENGINE_MATH_H
#define ENGINE_MATH_H

/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies.
* Erin Catto makes no representations about the suitability
* of this software for any purpose.
* It is provided "as is" without express or implied warranty.
*/

#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdlib.h>

//const float k_pi = 3.14159265358979323846264f;
const float angToReg = 57.324840764331210191082802547771f;

#ifndef M_PI
#define M_PI 3.14159265358979323846264f
#endif

struct Vec2
{
	Vec2() {}
	Vec2(float x, float y) : x(x), y(y) {}

	void Clear() { x = 0; y = 0; }
	void Set(float x_, float y_) { x = x_; y = y_; }

	Vec2 operator -() { return Vec2(-x, -y); }

	void operator += (const Vec2& v)
	{
		x += v.x; y += v.y;
	}

	void operator -= (const Vec2& v)
	{
		x -= v.x; y -= v.y;
	}

	void operator *= (float a)
	{
		x *= a; y *= a;
	}

	float Length() const
	{
		return sqrtf(x * x + y * y);
	}
	/*
Vec2 GetPointOnLine(Vec2 &fP,Vec2 &lP){
//double u = ((x - fP.X()) * (lP.X() - fP.X()) + (y - fP.Y()) * (lP.Y() - fP.Y())) / (lP.SickDistance2D(fP));
double u = (((*this) - fP) * (lP - fP))/ ((fP - lP) * (fP - lP));
return fP + (lP - fP) * u;
}
								  */
	float x, y;
};

struct Mat22
{
	Mat22() {}
	Mat22(float angle)
	{
		float c = cosf(angle), s = sinf(angle);
		col1.x = c; col2.x = -s;
		col1.y = s; col2.y = c;
	}

	Mat22(const Vec2& col1, const Vec2& col2) : col1(col1), col2(col2) {}

	Mat22 Transpose() const
	{
		return Mat22(Vec2(col1.x, col2.x), Vec2(col1.y, col2.y));
	}

	Mat22 Invert() const
	{
		float a = col1.x, b = col2.x, c = col1.y, d = col2.y;
		Mat22 B;
		float det = a * d - b * c;
		assert(det != 0.0f);
		det = 1.0f / det;
		B.col1.x = det * d;	B.col2.x = -det * b;
		B.col1.y = -det * c;	B.col2.y = det * a;
		return B;
	}

	Vec2 col1, col2;
};

inline float Dot(const Vec2& a, const Vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float Cross(const Vec2& a, const Vec2& b)
{
	return a.x * b.y - a.y * b.x;
}

inline Vec2 Cross(const Vec2& a, float s)
{
	return Vec2(s * a.y, -s * a.x);
}

inline Vec2 Cross(float s, const Vec2& a)
{
	return Vec2(-s * a.y, s * a.x);
}

inline Vec2 operator * (const Mat22& A, const Vec2& v)
{
	return Vec2(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}

inline Vec2 operator + (const Vec2& a, const Vec2& b)
{
	return Vec2(a.x + b.x, a.y + b.y);
}

inline Vec2 operator - (const Vec2& a, const Vec2& b)
{
	return Vec2(a.x - b.x, a.y - b.y);
}

inline Vec2 operator * (float s, const Vec2& v)
{
	return Vec2(s * v.x, s * v.y);
}

inline Mat22 operator + (const Mat22& A, const Mat22& B)
{
	return Mat22(A.col1 + B.col1, A.col2 + B.col2);
}

inline Mat22 operator * (const Mat22& A, const Mat22& B)
{
	return Mat22(A * B.col1, A * B.col2);
}

inline float Abs(float a)
{
	return a > 0.0f ? a : -a;
}

inline Vec2 Abs(const Vec2& a)
{
	return Vec2(fabsf(a.x), fabsf(a.y));
}

inline Mat22 Abs(const Mat22& A)
{
	return Mat22(Abs(A.col1), Abs(A.col2));
}

inline float Sign(float x)
{
	return x < 0.0f ? -1.0f : 1.0f;
}

inline float Min(float a, float b)
{
	return a < b ? a : b;
}

inline float Max(float a, float b)
{
	return a > b ? a : b;
}

inline float Clamp(float a, float low, float high)
{
	return Max(low, Min(a, high));
}

template<typename T> inline void Swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

// Random number in range [-1,1]


inline float Random()
{
	float r = (float)rand();
	r /= RAND_MAX;
	r = 2.0f * r - 1.0f;
	return r;
}

inline float Random(float lo, float hi)
{
	float r = (float)rand();
	r /= RAND_MAX;
	r = (hi - lo) * r + lo;
	return r;
}

inline int RandomChoose(int lo, int hi)
{
	return  (int)min(hi, max(lo, (int)Random(lo - 1, hi + 1)));
}

/*
double point_direction( double x1, double y1, double x2, double y2 )
{

}
	  */
inline float negative(float x) {
	if (x < 0) return -1; else return 1;
}

inline float pointDirection(const Vec2 a, const Vec2 b)
{
	return 180 - (atan2(a.y - b.y, a.x - b.x) / M_PI * 180);
}

inline float pointDistance(const Vec2 a, const Vec2 b)
{
	return (float)sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

inline float lengthdirX(float len, float dir)
{
	return cos(dir * M_PI / 180) * len;
}

inline float lengthdirY(float len, float dir)
{
	return -sin(dir * M_PI / 180) * len;
}

inline Vec2 lengthdir(float len, float dir) {
	return Vec2(lengthdirX(len, dir), lengthdirY(len, dir));
}

inline float dirDiv(float argument0, float argument1)
{

	if (argument1 > argument0 + 180.0f)
		return -((360.0f - argument1) + argument0);
	else
	{
		if (argument0 > argument1 + 180.0f)
			return ((360.0f - argument0) + argument1);
		else
			return argument1 - argument0;
	}

}



inline double dotProduct(const Vec2& a, const Vec2& b) {
	return a.x * b.x + a.y * b.y;
}

inline double magnitude(const Vec2& p) {
	return std::sqrt(p.x * p.x + p.y * p.y);
}

inline Vec2 vectorSubtraction(const Vec2& a, const Vec2& b) {
	Vec2 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

inline Vec2 vectorProjection(const Vec2& a, const Vec2& b) {
	double scalarProjection = dotProduct(a, b) / magnitude(b);
	Vec2 projection;
	projection.x = scalarProjection * (b.x / magnitude(b));
	projection.y = scalarProjection * (b.y / magnitude(b));
	return projection;
}


/*
inline float StringToFloat(string Sequence, float Time, float Limit) {
	int L = 0;
	int H = Sequence.size();

	float progres = Time * 1.00000f / Limit;
	int place = max(min((int)(progres * H), H - 1), L);
	int current = (int)Sequence.at(place);

	if (current >= 65 && current <= 90) // from A to Z
	{
		current -= 65;
		//  cout<<"StrToFL:"<< Sequence.c_str() <<" / "<<progres<<" /  (" << place <<"/"<< H <<")  / "<< current <<"  / "<<endl;
		return (current * 1.00000f / 25);
	}
	else
		if (current >= 97 && current <= 122) // from a to z
		{
			current -= 97;
			int nextplace = place + 1; if (nextplace > H - 1) nextplace = 0;
			nextplace = max(min(nextplace, H - 1), L);
			int next = (int)Sequence.at(nextplace);
			if (next >= 97 && next <= 122) next -= 97;
			if (next >= 65 && next <= 90) next -= 65;
			float minf, maxf, f;
			minf = 1.00000f * place / H;
			maxf = 1.00000f * (place + 1) / H;

			f = 1.00000f * (progres - minf) / (maxf - minf);
			float blend = current + (next - current) * f;
			//cout<<"StrToFL:"<< Sequence.c_str() <<" / "<<progres<<" /  (" << place <<"/"<< H-1 <<")  /   "<< current <<"|"<<next<<"("<<f<<")" << endl;
			return (blend * 1.00000f / 25);

			//return (current*1.00000f/25);
		}

	current-=65;
	if (current<0) current = 0;
	if (current>90) current = 90;

	//cout<<"StrToFL:"<< Sequence.c_str() <<" / "<<progres<<" /  (" << place <<"/"<< H <<")  / "<< current <<"  / "<<endl;
	return (current*1.00000f/25);
*/



#endif // ENGINE_MATH_H
