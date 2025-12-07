#pragma once
#include <iostream>
#include <algorithm>
#include"GamesEngineeringBase.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include"GEMLoader.h"  //我既然都要用GEMVector3了，那就得包含这个头文件了，为什么不行
#undef min
#undef max

#define SQ(x) ((x) * (x))
template<typename T>
static T clamp(const T value, const T minValue, const T maxValue)
{
	return std::max(std::min(value, maxValue), minValue);
}
template<typename t>
t simpleInterpolateAttribute(t a0, t a1, t a2, float alpha, float beta, float gamma) {
	return (a0 * alpha) + (a1 * beta) + (a2 * gamma);
}
class Colour
{

public:
	union
	{
		struct { float r, g, b, a; };
		float c[4];
	};
	Colour(float _r = 0, float _g = 0, float _b = 0, float _a = 1) :r(_r), g(_g), b(_b), a(_a) {};
	Colour operator+(const Colour& colour) const
	{
		return Colour(r + colour.r, g + colour.g, b + colour.b, a + colour.a);
	}
	Colour operator*(const Colour& colour) const
	{
		return Colour(r * colour.r, g * colour.g, b * colour.b, a * colour.a);
	}
	Colour operator*(const float s) const
	{
		return Colour(r * s, g * s, b * s, a * s);
	}
	Colour operator/(const float s) const {
		return Colour(r / s, g / s, b / s, a / s);
	}
};
template<typename T>
static T lerp(const T a, const T b, float t)
{
	return a + (b - a) * t;
}class Vec3
{
public:
	union
	{
		float v[3];
		struct { float x, y, z; };
	};
	Vec3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {};  //默认造函数需要三个参数,默认值写在声明里
	Vec3 operator+(const Vec3& pVec) const
	{
		return Vec3(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2]);
	};
	Vec3& operator=(const GEMLoader::GEMVec3& gemVec) {  //重载赋值运算符以支持GEMVector3类型的赋值,注意namespace可能不一样
		x = gemVec.x;
		y = gemVec.y;
		z = gemVec.z;
		return *this;
	}
	Vec3& operator+=(const Vec3& pVec)
	{
		v[0] += pVec.v[0];
		v[1] += pVec.v[1];
		v[2] += pVec.v[2];
		return *this;
	};
	Vec3& operator-=(const Vec3& pVec)
	{
		v[0] -= pVec.v[0];
		v[1] -= pVec.v[1];
		v[2] -= pVec.v[2];
		return *this;
	};
	Vec3 operator-() const  //成员函数重载是允许重载一元运算符的，这个时候不需要传递任何参数，直接作用于自己本身，相当于放在自己obj的前面
	{
		// 直接使用 *this 的 x, y, z
		return Vec3(-x, -y, -z);
	}
	Vec3 operator- (const Vec3& pVec) const {
		return Vec3(v[0] - pVec.v[0], v[1] - pVec.v[1], v[2] - pVec.v[2]);
	}
	Vec3 operator/(const Vec3& pVec) const
	{
		return Vec3(v[0] / pVec.v[0], v[1] / pVec.v[1], v[2] / pVec.v[2]);   //返回一个新的对象
	};
	Vec3& operator/=(const Vec3& pVec)  //所有的复合赋值运算符都返回当前对象的引用，以支持链式操作和修改原对象的值，
	{
		v[0] /= pVec.v[0];
		v[1] /= pVec.v[1];
		v[2] /= pVec.v[2];
		return *this;  //返回的就是当前对象的引用，而不是一个新对象会产生不必要的拷贝
	}  //我返回引用是为了支持链式操作？什么意思
	Vec3 operator/(float scalar) const
	{
		return Vec3(v[0] / scalar, v[1] / scalar, v[2] / scalar);
	}
	Vec3& operator/=(float sacalar)
	{
		v[0] /= sacalar;
		v[1] /= sacalar;
		v[2] /= sacalar;
		return *this;
	}
	Vec3 operator*(float scalar) const
	{
		return Vec3(v[0] * scalar, v[1] * scalar, v[2] * scalar);
	}
	Vec3& operator*=(float scalar)
	{
		v[0] *= scalar;
		v[1] *= scalar;
		v[2] *= scalar;
		return *this;
	}
	Vec3& operator*=(const Vec3& pVec)
	{
		v[0] = pVec.v[0];
		v[1] = pVec.v[1];
		v[2] = pVec.v[2];
		return *this;
	}  //如果返回一个对象，实际是在栈上创建了一个临时对象，然后将其拷贝给调用者，而返回*this引用则是返回当前对象本身的引用，所以返回引用常用于修改当前对象自身的值
	float Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	float LengthSq() const
	{
		return x * x + y * y + z * z;
	}
	Vec3 normalize(void)
	{
		float len = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		return *this * len;
	}
	float normalize_GetLength()
	{
		float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		float len = 1.0f / length;
		v[0] *= len; v[1] *= len; v[2] *= len;
		return length;
	}
	float Dot(const Vec3& pVec) const  //也可以在类外写成友元函数
	{
		return v[0] * pVec.v[0] + v[1] * pVec.v[1] + v[2] * pVec.v[2];
	}
	Vec3 Cross(const Vec3& v1)
	{
		return Vec3(v1.v[1] * v[2] - v1.v[2] * v[1],
			v1.v[2] * v[0] - v1.v[0] * v[2],
			v1.v[0] * v[1] - v1.v[1] * v[0]);
	}
	void Scale()
	{

	}
	friend const std::ostream& operator <<(std::ostream& os, const Vec3& vec);  //申明为友元函数，可以访问私有成员
};  //un ion实际上是在说明这个类的内存布局，可以通过v数组或者x,y,z来访问数据，但不能同时使用两种方式存储不同的数据
const std::ostream& operator <<(std::ostream& os, const Vec3& vec)
{
	os << "Vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;  //为了支持链式输出必须使用引用，只读也可以
}
Vec3 Max(const Vec3& v1, const  Vec3& v2)  //这是类外函数但是访问了类的公有成员，因为v数组是公有的所以类外
{
	return Vec3(std::max(v1.v[0], v2.v[0]), std::max(v1.v[1], v2.v[1]), std::max(v1.v[2], v2.v[2]));
}

class Vec4
{
public:
	union
	{
		float v[4];
		struct { float x, y, z, w; };

	};
	Vec4(float _x = 0, float _y = 0, float _z = 0, float _w = 1) :x(_x), y(_y), z(_z), w(_w) {};  //_w是有默认参数的，不传参默认为1，如果没有默认参数代表实例化时值必须传参赋值
	Vec4 operator+(const Vec4& pVec) const
	{
		return Vec4(v[0] + pVec.v[0], v[1] + pVec.v[1], v[2] + pVec.v[2] + v[3] + pVec.v[3]);
	};
	Vec4& operator+=(const Vec4& pVec)
	{
		v[0] += pVec.v[0];
		v[1] += pVec.v[1];
		v[2] += pVec.v[2];
		v[3] += pVec.v[3];
		return *this;
	};
	Vec4 operator/(const Vec4& pVec) const
	{
		return Vec4(v[0] / pVec.v[0], v[1] / pVec.v[1], v[2] / pVec.v[2], v[3] / pVec.v[3]);   //返回一个新的对象
	};
	Vec4& operator/=(const Vec4& pVec)  //所有的复合赋值运算符都返回当前对象的引用，以支持链式操作和修改原对象的值，
	{
		v[0] /= pVec.v[0];
		v[1] /= pVec.v[1];
		v[2] /= pVec.v[2];
		v[3] /= pVec.v[3];
		return *this;  //返回的就是当前对象的引用，而不是一个新对象会产生不必要的拷贝
	}  //我返回引用是为了支持链式操作？什么意思
	Vec4 operator/(float scalar) const
	{
		return Vec4(v[0] / scalar, v[1] / scalar, v[2] / scalar, v[3] / scalar);
	}
	Vec4& operator/=(float sacalar)
	{
		v[0] /= sacalar;
		v[1] /= sacalar;
		v[2] /= sacalar;
		v[3] /= sacalar;
		return *this;
	}
	Vec4 operator*(float scalar) const
	{
		return Vec4(v[0] * scalar, v[1] * scalar, v[2] * scalar, v[3] * scalar);
	}
	Vec4& operator*=(float scalar)
	{
		v[0] *= scalar;
		v[1] *= scalar;
		v[2] *= scalar;
		v[3] *= scalar;
		return *this;
	}
	Vec4& operator*=(const Vec4& pVec)
	{
		v[0] = pVec.v[0];
		v[1] = pVec.v[1];
		v[2] = pVec.v[2];
		v[3] = pVec.v[3];
		return *this;
	}
	Vec4& DevideByW()
	{
		v[0] /= v[3];
		v[1] /= v[3];
		v[2] /= v[3];
		v[3] = 1.0f;  //此时得到的点是真实的三维坐标

		return *this;
	}
};
class Triangle
{
	Vec4 Point_1;
	Vec4 Point_2;
	Vec4 Point_3;
	int width = 0;
	int height = 0;
	Vec4 tr = 0;  //triangle right top
	Vec4 bl = 0;  //triangle bottom left  //;要在类内通过成员方法赋值成员变量
public:
	Triangle(float x_1, float y_1, float x_2, float y_2, float x_3, float y_3, int windowwidth, int windowheight) :Point_1(x_1, y_1), Point_2(x_2, y_2), Point_3(x_3, y_3), width(windowwidth), height(windowheight) {};
	float edgeFunction(const Vec4& v0, const Vec4& v1, const Vec4& p)  //  v0v1是三角形上的两个顶点  //变为静态函数后就不需要实例化对象，直接穿要计算的三角形的点
	{
		return (((p.x - v0.x) * (v1.y - v0.y)) - ((v1.x - v0.x) * (p.y - v0.y)));  //这里无法判断是否在三角形内把
	}
	void findBounds(const Vec4& v0, const Vec4& v1, const Vec4& v2, Vec4& tr, Vec4& bl)
	{
		//GamesEngineeringBase::Window canvas;
		tr.x = std::min(static_cast<int>(std::max(std::max(v0.x, v1.x), v2.x)), width - 1);
		tr.y = std::min(static_cast<int>(std::max(std::max(v0.y, v1.y), v2.y)), height - 1);
		bl.x = std::max(static_cast<int>(std::min(std::min(v0.x, v1.x), v2.x)), 0);
		bl.y = std::max(static_cast<int>(std::min(std::min(v0.y, v1.y), v2.y)), 0);
	}
	//bary是用来计算三角形面积的

	void CalculateTriangle(GamesEngineeringBase::Window& canvas)
	{
		findBounds(Point_1, Point_2, Point_3, tr, bl);  //先计算三角形的包围盒
		for (int y = bl.y; y <= tr.y; y++)  //计算三角形的包围盒中的每个像素的位置
			for (int x = bl.x; x <= tr.x; x++) {
				Vec4 p(x + 0.5f, y + 0.5f);
				// Compute triangle here
				float projArea = edgeFunction(Point_1, Point_2, Point_3);
				float area = 1.0f / projArea;
				float alpha = edgeFunction(Point_2, Point_3, p);  //将点与三角形的每条边进行比较
				float beta = edgeFunction(Point_3, Point_1, p);
				float gamma = edgeFunction(Point_1, Point_2, p);


				alpha *= area;
				beta *= area;
				gamma *= area;
				Colour frag = simpleInterpolateAttribute(Colour(1.0f, 0, 0), Colour(0, 1.0f, 0), Colour(0, 0, 1.0f),
					alpha, beta, gamma);
				if (alpha > 0 && beta > 0 && gamma > 0 && alpha + beta + gamma <= 1)
				{
					canvas.draw(x, y, frag.r * 255, frag.g * 255, frag.b * 255);
				}

			}

	}
};

class Matrix
{
public:
	union
	{
		float a[4][4];
		float m[16];
	};
	Matrix()  //默认构造函数不传参就直接在内部初始化值
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (i == j) a[i][j] = 1.0f;
				else a[i][j] = 0.0f;
			}
		}
	}
	Vec4 mul(const Vec4& v)
	{
		return Vec4(
			(v.x * m[0] + v.y * m[1] + v.z * m[2] + v.w * m[3]),
			(v.x * m[4] + v.y * m[5] + v.z * m[6] + v.w * m[7]),
			(v.x * m[8] + v.y * m[9] + v.z * m[10] + v.w * m[11]),
			(v.x * m[12] + v.y * m[13] + v.z * m[14] + v.w * m[15]));
	}
	Vec3 mulPoint(const Vec3& v)  //这是w=1的点平移变换，注意点是会被平移的，但是方向是不能被平移的，因为你的方向本身就是要表示一个确定的方法
	{
		Vec3 v1 = Vec3(
			(v.x * m[0] + v.y * m[1] + v.z * m[2]) + m[3],
			(v.x * m[4] + v.y * m[5] + v.z * m[6]) + m[7],
			(v.x * m[8] + v.y * m[9] + v.z * m[10]) + m[11]);
		return v1;
	}
	Vec3 mulVec(const Vec3& v)  //这是w=0的向量变换，只做了旋转和缩放，没有平移，用来保存向量的方向，一般来说还是用Vec3更方便
	{
		return Vec3(
			(v.x * m[0] + v.y * m[1] + v.z * m[2]),
			(v.x * m[4] + v.y * m[5] + v.z * m[6]),
			(v.x * m[8] + v.y * m[9] + v.z * m[10]));
	}
	Vec3 Translation(const Vec3& v)
	{  //treanslation矩阵乘以点不就是平移吗，然后我就只需要返回平移后的点就行了，任何矩阵都可以作为平移矩阵使用，只要最后一列是平移值就行
		return Vec3((v.x * m[0] + v.y * m[1] + v.z * m[2]) + m[3],
			(v.x * m[4] + v.y * m[5] + v.z * m[6]) + m[7],
			(v.x * m[8] + v.y * m[9] + v.z * m[10]) + m[11]);
	}
	Matrix static ScaleMatrix(const Vec3& v)  //我重写了缩放矩阵，穿的是缩放向量，返回一个缩放矩阵
	{
		Matrix ret;  //默认构造函数已经初始化为单位矩阵，你只需要改对角线上的值就行了
		ret.m[0] = v.x;
		ret.m[5] = v.y;
		ret.m[10] = v.z;
		return ret;  //相当于要sacle之前先实例化这个缩放矩阵，然后再调用scalevec方法左乘向量
	}
	Vec3 ScaleVec(const Vec3& v)  //这样写的前提是m必须是纯缩放矩阵，也就是目前这个调用对象必须是一个缩放矩阵
	{
		return Vec3(
			v.x * m[0],  //缩放矩阵乘以向量就是每个分量分别乘以对应轴的缩放值，但是我这个矩阵就一定会要是纯缩放矩阵吗？我不可以假设存在这样一个矩阵吗
			v.y * m[5],
			v.z * m[10]);
	}
	//Matrix mul(const Matrix& matrix) const
	//{
	   // Matrix ret;
	   // ret.m[0] = m[0] * matrix.m[0] + m[1] * matrix.m[4] + m[2] * matrix.m[8] + m[3] * matrix.m[12];  //尽量少三层循环
	   // ret.m[1] = m[0] * matrix.m[1] + m[1] * matrix.m[5] + m[2] * matrix.m[9] + m[3] * matrix.m[13];
	   // ret.m[2] = m[0] * matrix.m[2] + m[1] * matrix.m[6] + m[2] * matrix.m[10] + m[3] * matrix.m[14];
	   // ret.m[3] = m[0] * matrix.m[3] + m[1] * matrix.m[7] + m[2] * matrix.m[11] + m[3] * matrix.m[15];
	   // ret.m[4] = m[4] * matrix.m[0] + m[5] * matrix.m[4] + m[6] * matrix.m[8] + m[7] * matrix.m[12];
	   // ret.m[5] = m[4] * matrix.m[1] + m[5] * matrix.m[5] + m[6] * matrix.m[9] + m[7] * matrix.m[13];
	   // ret.m[6] = m[4] * matrix.m[2] + m[5] * matrix.m[6] + m[6] * matrix.m[10] + m[7] * matrix.m[14];
	   // ret.m[7] = m[4] * matrix.m[3] + m[5] * matrix.m[7] + m[6] * matrix.m[11] + m[7] * matrix.m[15];
	   // ret.m[8] = m[8] * matrix.m[0] + m[9] * matrix.m[4] + m[10] * matrix.m[8] + m[11] * matrix.m[12];
	   // ret.m[9] = m[8] * matrix.m[1] + m[9] * matrix.m[5] + m[10] * matrix.m[9] + m[11] * matrix.m[13];
	   // ret.m[10] = m[8] * matrix.m[2] + m[9] * matrix.m[6] + m[10] * matrix.m[10] + m[11] * matrix.m[14];
	   // ret.m[11] = m[8] * matrix.m[3] + m[9] * matrix.m[7] + m[10] * matrix.m[11] + m[11] * matrix.m[15];
	   // ret.m[12] = m[12] * matrix.m[0] + m[13] * matrix.m[4] + m[14] * matrix.m[8] + m[15] * matrix.m[12];
	   // ret.m[13] = m[12] * matrix.m[1] + m[13] * matrix.m[5] + m[14] * matrix.m[9] + m[15] * matrix.m[13];
	   // ret.m[14] = m[12] * matrix.m[2] + m[13] * matrix.m[6] + m[14] * matrix.m[10] + m[15] * matrix.m[14];
	   // ret.m[15] = m[12] * matrix.m[3] + m[13] * matrix.m[7] + m[14] * matrix.m[11] + m[15] * matrix.m[15];
	   // return ret;
	//}

	Vec3 Scale(const Vec3& v)
	{
		return Vec3(
			v.v[0] * a[0][0] + v.v[1] * a[0][1] + v.v[2] * a[0][2],
			v.v[0] * a[1][0] + v.v[1] * a[1][1] + v.v[2] * a[1][2],
			v.v[0] * a[2][0] + v.v[1] * a[2][1] + v.v[2] * a[2][2]
			//实际上需要返回的只有前三个分量组成的Vec3
		);
	}
	Matrix static RotationMatrixX(float angle)
	{
		Matrix ret;
		float cin = cosf(angle);  //cos接受的是double类型参数，cosf接受float
		float sin = sinf(angle);
		ret.m[5] = cin; ret.m[6] = -sin;
		ret.m[9] = sin; ret.m[10] = cin;
		return ret;
	}
	Vec3 RotationBaseX(const Vec3& v)  //这里假设是绕x轴顺时针旋转，并没有产生实际的矩阵对象
	{
		return Vec3(
			v.v[0] * m[0] + v.v[1] * m[4] + v.v[2] * m[8], //x基准坐标轴不变
			v.v[0] * m[1] + v.v[1] * m[5] + v.v[3] * m[9],  //y' = ycosθ - zsinθ，因为另外两轴x值都为0所以不变
			v.v[0] * m[2] + v.v[1] * m[6] + v.v[3] * m[10]  //绕x轴旋转矩阵左乘向量
		);
	}
	Matrix static RotationMatrixY(float angle)
	{
		Matrix ret;
		float cin = cosf(angle);
		float sin = sinf(angle);
		ret.m[0] = cin; ret.m[2] = sin;
		ret.m[8] = -sin; ret.m[10] = cin;
		return ret;
	}
	Vec3 RotationBaseY(const Vec3& v)  //绕y轴旋转矩阵左乘向量
	{
		return Vec3(
			v.v[0] * m[0] + v.v[1] * m[4] + v.v[2] * m[8],
			v.v[0] * m[1] + v.v[1] * m[5] + v.v[2] * m[9],  //y基准坐标轴不变
			v.v[0] * m[2] + v.v[1] * m[6] + v.v[2] * m[10]
		);
	}
	Matrix static RotationMatrixZ(float angle)  //绕z轴旋转矩阵 ，先有变化矩阵后有左乘向量的方法
	{
		Matrix ret;
		float cin = cosf(angle);
		float sin = sinf(angle);
		ret.m[0] = cin; ret.m[1] = -sin;
		ret.m[4] = sin; ret.m[5] = cin;
		return ret;
	}
	static Matrix translation(const Vec3& t)  //这个就是我的平移矩阵
	{
		Matrix result;                     // 默认构造应该是单位矩阵
		result.m[3] = t.x;                // 第一行末列 = X平移
		result.m[7] = t.y;                // 第二行末列 = Y平移
		result.m[11] = t.z;                // 第三行末列 = Z平移
		return result;
	}
	Vec3 RotationBaseZ(const Vec3& v)  //绕z轴旋转矩阵左乘向量
	{
		return Vec3(
			v.v[0] * m[0] + v.v[1] * m[4] + v.v[2] * m[8],
			v.v[0] * m[1] + v.v[1] * m[5] + v.v[2] * m[9],
			v.v[0] * m[2] + v.v[1] * m[6] + v.v[2] * m[10]  //z基准坐标轴不变
		);
	}
	Matrix mul(const Matrix& matrix) const  //这个矩阵乘法确定了乘法顺序是 this * matrix
	{//现在的这个矩阵乘法得到的结果是this矩阵在前，matrix矩阵在后，也就是说我这个矩阵的变换是先执行matrix的变换再执行this的变换，所以需要右乘列向量
		Matrix ret;
		ret.m[0] = m[0] * matrix.m[0] + m[4] * matrix.m[1] + m[8] * matrix.m[2] + m[12] * matrix.m[3];
		ret.m[1] = m[1] * matrix.m[0] + m[5] * matrix.m[1] + m[9] * matrix.m[2] + m[13] * matrix.m[3];
		ret.m[2] = m[2] * matrix.m[0] + m[6] * matrix.m[1] + m[10] * matrix.m[2] + m[14] * matrix.m[3];
		ret.m[3] = m[3] * matrix.m[0] + m[7] * matrix.m[1] + m[11] * matrix.m[2] + m[15] * matrix.m[3];

		ret.m[4] = m[0] * matrix.m[4] + m[4] * matrix.m[5] + m[8] * matrix.m[6] + m[12] * matrix.m[7];
		ret.m[5] = m[1] * matrix.m[4] + m[5] * matrix.m[5] + m[9] * matrix.m[6] + m[13] * matrix.m[7];
		ret.m[6] = m[2] * matrix.m[4] + m[6] * matrix.m[5] + m[10] * matrix.m[6] + m[14] * matrix.m[7];
		ret.m[7] = m[3] * matrix.m[4] + m[7] * matrix.m[5] + m[11] * matrix.m[6] + m[15] * matrix.m[7];

		ret.m[8] = m[0] * matrix.m[8] + m[4] * matrix.m[9] + m[8] * matrix.m[10] + m[12] * matrix.m[11];
		ret.m[9] = m[1] * matrix.m[8] + m[5] * matrix.m[9] + m[9] * matrix.m[10] + m[13] * matrix.m[11];
		ret.m[10] = m[2] * matrix.m[8] + m[6] * matrix.m[9] + m[10] * matrix.m[10] + m[14] * matrix.m[11];
		ret.m[11] = m[3] * matrix.m[8] + m[7] * matrix.m[9] + m[11] * matrix.m[10] + m[15] * matrix.m[11];

		ret.m[12] = m[0] * matrix.m[12] + m[4] * matrix.m[13] + m[8] * matrix.m[14] + m[12] * matrix.m[15];
		ret.m[13] = m[1] * matrix.m[12] + m[5] * matrix.m[13] + m[9] * matrix.m[14] + m[13] * matrix.m[15];
		ret.m[14] = m[2] * matrix.m[12] + m[6] * matrix.m[13] + m[10] * matrix.m[14] + m[14] * matrix.m[15];
		ret.m[15] = m[3] * matrix.m[12] + m[7] * matrix.m[13] + m[11] * matrix.m[14] + m[15] * matrix.m[15];

		return ret;
	}
	Matrix operator*(const Matrix& matrix)
	{
		return this->mul(matrix);  //重载乘法运算符，调用mul方法
	}
	Matrix invert()  //得到的是当前矩阵的逆矩阵
	{
		Matrix inv;
		inv.m[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
		inv.m[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
		inv.m[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
		inv.m[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
		inv.m[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
		inv.m[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
		inv.m[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
		inv.m[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
		inv.m[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
		inv.m[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
		inv.m[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
		inv.m[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
		inv.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
		inv.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
		inv.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
		inv.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
		float det = m[0] * inv.m[0] + m[1] * inv.m[4] + m[2] * inv.m[8] + m[3] * inv.m[12];
		if (det == 0) {
			// Handle this case 
		}
		det = 1.0 / det;
		for (int i = 0; i < 16; i++) {
			inv.m[i] = inv.m[i] * det;
		}
		return inv;
	}
	Matrix static ProjectionMatrix(float angle,float aspect, float near_plane,float far_plane)  //这个方法返回一个透视投影矩阵，可以在下面再写一个乘向量的方法
	{
		//必须要有宽高比参数才能确保计算完y的拉伸之后算出x轴的拉伸
		Matrix ret;
		angle *= (3.1415f / 180);
		ret.m[0] = 1.0f / std::tanf(angle / 2.0f) / aspect;  //fov是垂直视野角度，注意这里的angle要以弧度为单位
		ret.m[5] = 1.0f / std::tanf(angle / 2.0f);
		ret.m[10] = far_plane / (far_plane - near_plane);
		ret.m[11] = (-far_plane * near_plane) / (far_plane - near_plane);  //这个矩阵的第三行假设的是朝-z看，即只有负数被投影矩阵会变到正数空间？
		ret.m[14] = 1.0f;   //这里是透视投影矩阵的关键，w分量变成了-z，d3d12用的左手坐标系
		return ret;
	}
	Matrix Transpose() const
	{
		Matrix r = *this;

		r.m[0] = m[0];
		r.m[1] = m[4];
		r.m[2] = m[8];
		r.m[3] = m[12];

		r.m[4] = m[1];
		r.m[5] = m[5];
		r.m[6] = m[9];
		r.m[7] = m[13];

		r.m[8] = m[2];
		r.m[9] = m[6];
		r.m[10] = m[10];
		r.m[11] = m[14];

		r.m[12] = m[3];
		r.m[13] = m[7];
		r.m[14] = m[11];
		r.m[15] = m[15];

		return r;
	}
	Matrix static Lookat(const Vec3& CameraPositon,const Vec3& to,const Vec3& up)  //返回的是自动生成的视图矩阵
	{
		Matrix ret;
		Vec3 forward = (to - CameraPositon).normalize();  //这里的forward是从相机位置指向目标位置的方向向量
		Vec3 right = forward.Cross(up).normalize();  //这里我默认传入的forward和up都是单位向量且正交的  ，注意这里是左手坐标系
		Vec3 up_ = right.Cross(forward).normalize();  //重新计算正交的up向量，确保三个方向都是正交的且是单位向量,参数是四指朝向,所以我这里参数是四指方向，因为原本是为了右手坐标系
		ret.m[0] = right.x, ret.m[1] = right.y, ret.m[2] = right.z, ret.m[3] = -CameraPositon.Dot(right);
		ret.m[4] = up_.x, ret.m[5] = up_.y, ret.m[6] = up_.z, ret.m[7] = -CameraPositon.Dot(up_);
		ret.m[8] = forward.x, ret.m[9] = forward.y, ret.m[10] = forward.z, ret.m[11] = -CameraPositon.Dot(forward);  //这里必须反过来的原因是因为directx是左手坐标系，所以forward方向是正z轴方向
		return ret; //这里输出的就是视图矩阵，第四行不用单独设置，因为默认就是0 0 0 1
	}
};
class ShadingFrame
{
	Vec3 tangent;
	Vec3 bitangent;
	Vec3 normal;
public:
	ShadingFrame(const Vec3& t, const Vec3& b, const Vec3& n) :tangent(t), bitangent(b), normal(n) {}; //直接传入三个向量作为参数
	Vec3 LocalToWorld(const Vec3& localVec)
	{
		return Vec3(
			localVec.x * tangent.x + localVec.y * bitangent.x + localVec.z * normal.x, //从局部坐标系转换到世界坐标系，直接乘TBN矩阵就行了，注意x轴的值是每个基上的x分量相加，
			localVec.x * tangent.y + localVec.y * bitangent.y + localVec.z * normal.y,
			localVec.x * tangent.z + localVec.y * bitangent.z + localVec.z * normal.z
		);
	}
	Vec3 WorldToLocal(const Vec3& worldVec)
	{
		return Vec3(
			worldVec.Dot(tangent),
			worldVec.Dot(bitangent),
			worldVec.Dot(normal)
		);
	}
};

//class Quaternion
//{
//	union
//	{
//		struct { float x, y, z, w; }; // 这里 a,b,c 是向量部分，d 是标量部分
//		float q[4];
//	};
//public:
//	Quaternion(float _a = 0, float _b = 0, float _c = 0, float _w = 1) :x(_a), y(_b), z(_c), w(_w) {};
//	static Quaternion fromAxisAngle(const Vec3& axis, float angle)  //通过轴角度创建四元数表示旋转,angle以弧度为单位,返回一个归一的四元数对象
//	{
//		float halfAngle = angle * 0.5f;  //必须确保传入的轴是单位向量
//		float len = axis.Length();
//		float unitx = axis.x / len;
//		float unity = axis.y / len;
//		float unitz = axis.z / len;
//		float sinHalfAngle = std::sinf(halfAngle);
//		return Quaternion(
//			unitx * sinHalfAngle,
//			unity * sinHalfAngle,
//			unitz * sinHalfAngle,
//			std::cosf(halfAngle)
//		);
//	}
//	float magnitude(const Quaternion& q)
//	{
//		return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
//	}
//	Quaternion normalize()  //返回一个新的单位四元数对象，将任意四元数归一化，表示旋转时必须使用单位四元数,对调用该方法的对象进行归一化
//	{
//		Quaternion q = *this;
//		float mag = magnitude(q);
//		return Quaternion(q.x / mag, q.y / mag, q.z / mag, q.w / mag);
//	}
//	Quaternion conjugate(const Quaternion& q)
//	{
//		return Quaternion(-q.x, -q.y, -q.z, q.w);
//	}
//	friend Quaternion operator*(const Quaternion& a, const Quaternion& b); //申明为友元把 operator* 函数放在类外定义，可以访问私有成员，这样就可以写两个参数
//	Quaternion slerp(const Quaternion& q0, const Quaternion& q1, float t)
//	{
//		// 假设 q0、q1 都已经归一化
//		// 1. 点积
//		float dot = q0.w * q1.w + q0.x * q1.x + q0.y * q1.y + q0.z * q1.z;
//
//		Quaternion q1Copy = q1;
//
//		// 2. 如果 dot < 0，翻转 q1，走最短路径
//		if (dot < 0.0f)
//		{
//			dot = -dot;
//			q1Copy.w = -q1Copy.w;
//			q1Copy.x = -q1Copy.x;
//			q1Copy.y = -q1Copy.y;
//			q1Copy.z = -q1Copy.z;
//		}
//
//		const float DOT_THRESHOLD = 0.9995f;
//		if (dot > DOT_THRESHOLD)
//		{
//			// 角度非常小，退化为 Lerp 再 Normalize
//			Quaternion result = Quaternion(
//				q0.w + t * (q1Copy.w - q0.w),
//				q0.x + t * (q1Copy.x - q0.x),
//				q0.y + t * (q1Copy.y - q0.y),
//				q0.z + t * (q1Copy.z - q0.z)
//			);
//			return result.normalize();
//		}
//
//		// 3. 真正的 Slerp
//		float theta = std::acos(dot);     // 夹角
//		float sinTheta = std::sin(theta);
//
//		float w0 = std::sin((1.0f - t) * theta) / sinTheta;
//		float w1 = std::sin(t * theta) / sinTheta;
//
//		Quaternion result = q0 * w0 + q1Copy * w1;
//		return result.normalize();       // 理论上已经是单位长度，但保险起见
//	}
//	Quaternion operator+(const Quaternion& r) const
//	{
//		return Quaternion(w + r.w, x + r.x, y + r.y, z + r.z);
//	}
//};
//Quaternion operator*(const Quaternion& a, const Quaternion& b)  //由于四元数乘法旋转复合不是交换律的，b代表先旋转，a代表后旋转
//{
//	return Quaternion(
//		// x
//		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
//		// y
//		a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
//		// z
//		a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
//		// w
//		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
//	);
//}
class Quaternion
{
public:
	union {
		struct {
			float a;
			float b;
			float c;
			float d;
		};
		float q[4];
	};
	Quaternion()
	{
		a = 0;
		b = 0;
		c = 0;
		d = 0;
	}
	Quaternion(float _x, float _y, float _z, float _w)
	{
		a = _x;
		b = _y;
		c = _z;
		d = _w;
	}
	float norm()
	{
		return sqrtf(SQ(a) + SQ(b) + SQ(c) + SQ(d));
	}
	void Normalize()
	{
		float n = 1.0f / sqrtf(SQ(a) + SQ(b) + SQ(c) + SQ(d));
		a *= n;
		b *= n;
		c *= n;
		d *= n;
	}
	void Conjugate()
	{
		a = -a;
		b = -b;
		c = -c;
	}
	void invert()
	{
		Conjugate();
		Normalize();
	}
	Quaternion operator*(Quaternion q1)
	{
		Quaternion v;
		v.a = ((d * q1.a) + (a * q1.d) + (b * q1.c) - (c * q1.b));
		v.b = ((d * q1.b) - (a * q1.c) + (b * q1.d) + (c * q1.a));
		v.c = ((d * q1.c) + (a * q1.b) - (b * q1.a) + (c * q1.d));
		v.d = ((d * q1.d) - (a * q1.a) - (b * q1.b) - (c * q1.c));
		return v;
	}
	Matrix toMatrix()
	{
		float aa = a * a;
		float ab = a * b;
		float ac = a * c;
		float bb = b * b;
		float cc = c * c;
		float bc = b * c;
		float da = d * a;
		float db = d * b;
		float dc = d * c;
		Matrix matrix; //多光标输入是shift+alt+鼠标左键或者可以直接加上下键来快速在下方多行同位置加入光标
		matrix.m[0] = 1.0f - 2.0f * (bb + cc);
		matrix.m[1] = 2.0f * (ab - dc);
		matrix.m[2] = 2.0f * (ac + db);
		matrix.m[3] = 0.0;
		matrix.m[4] = 2.0f * (ab + dc);
		matrix.m[5] = 1.0f - 2.0f * (aa + cc);
		matrix.m[6] = 2.0f * (bc - da);
		matrix.m[7] = 0.0;
		matrix.m[8] = 2.0f * (ac - db);
		matrix.m[9] = 2.0f * (bc + da);
		matrix.m[10] = 1.0f - 2.0f * (aa + bb);
		matrix.m[11] = 0.0;
		matrix.m[12] = 0;
		matrix.m[13] = 0;
		matrix.m[14] = 0;
		matrix.m[15] = 1;
		return matrix;
	}
	void rotateAboutAxis(Vec3 pt, float angle, Vec3 axis)
	{
		Quaternion q1, p, qinv;
		q1.a = sinf(0.5f * angle) * axis.x;
		q1.b = sinf(0.5f * angle) * axis.y;
		q1.c = sinf(0.5f * angle) * axis.z;
		q1.d = cosf(0.5f * angle);
		p.a = pt.x;
		p.b = pt.y;
		p.c = pt.z;
		p.d = 0;
		qinv = q1;
		qinv.invert();
		q1 = q1 * p;
		q1 = q1 * qinv;
		a = q1.a;
		b = q1.b;
		c = q1.c;
		d = q1.d;
	}
	static Quaternion slerp(Quaternion q1, Quaternion q2, float t)
	{
		Quaternion qr;
		float dp = q1.a * q2.a + q1.b * q2.b + q1.c * q2.c + q1.d * q2.d;
		Quaternion q11 = dp < 0 ? -q1 : q1;
		dp = dp > 0 ? dp : -dp;
		float theta = acosf(clamp(dp, -1.0f, 1.0f));
		if (theta == 0)
		{
			return q1;
		}
		float d = sinf(theta);
		float a = sinf((1 - t) * theta);
		float b = sinf(t * theta);
		float coeff1 = a / d;
		float coeff2 = b / d;
		qr.a = coeff1 * q11.a + coeff2 * q2.a;
		qr.b = coeff1 * q11.b + coeff2 * q2.b;
		qr.c = coeff1 * q11.c + coeff2 * q2.c;
		qr.d = coeff1 * q11.d + coeff2 * q2.d;
		qr.Normalize();
		return qr;
	}
	Quaternion operator-()
	{
		return Quaternion(-a, -b, -c, -d);
	}
};
