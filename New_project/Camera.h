#pragma once
#include"CGBase.h"
#include<iostream>
#include"GamesEngineeringBase.h"
#include"Window.h"
#include<algorithm>
//template <typename T>
//const T& clamp(const T& value, const T& low, const T& high)
//{
//	return (value < low) ? low : (value > high ? high : value);  //两个三目运算的组合
//}
class Camera
{
public:
	Vec3 Offset{ 0.f,7.f,-7.f };
	Vec3 camaraposition;  //如果要在类中初始化类成员对象，就用初始化列表
	//这是类初始化位置  //花括号是列表初始化 ，，你在一个类中初始化一个类对象？？这是不允许的因为类中只允许两种存在成员和方法，基本类型可以直接初始化，但是类成员不行，因为括号会被自动解析为函数
	float yaw = 0.0f;  //我这个应该是绕y轴旋转角度把
	float pitch = 0.0f;
	float cameraanglespeed = 0.015f;  //这个就是相机的映射比例
	const float MAX_YAW_RAD = M_PI / 3.0f;
	Matrix getViewMatrix(const Vec3& heroposition)
	{
		  //相机的位置应该等于英雄位置减去偏移量  ，由于每帧heropositiuon都不一样所以这个也不一样
		Vec3 forward(
			cosf(pitch) * sinf(yaw),//yaw代表的就是在水平方向上的旋转度数
			sinf(pitch),
			cosf(pitch) * cosf(yaw)  //其实这是直接调用构造函数来进行初始化
		);  //这个用来表示相机旋转时自己位置的偏移量，这里为什么要在方法类声明forward是因为每次调用的时候，forward的值必须用最新值，如果你要保证最新值，你还要把yaw作为最新参数传进来
		//Matrix::RotationMatrixY(yaw)
		Vec3 rotatedOffset =
			Matrix::RotationMatrixY(yaw).mulPoint(Offset);  //这是将偏移量围绕y轴旋转yaw角度后的新偏移量，加上这个偏移量才是正确的相机位置
		camaraposition = heroposition + Vec3(0, 0, 5) + rotatedOffset;
		return  Matrix::Lookat(camaraposition, heroposition + Vec3(0,0,5), Vec3(0, 1, 0));  //这个就是初始的视图矩阵 ，记住我现在所有的变化的都是先变化的下载左边
		//Matrix::Lookat(camaraposition, heroposition + forward, Vec3(0, 1, 0));  //这个就是初始的视图矩阵
	}
	void updateCameraPosition(const Window& canvas, float deltaTime,float dx,float dy)
	{
		
		pitch += dy * cameraanglespeed;
		yaw += dx * cameraanglespeed;  //这里的dx是鼠标在x轴上的移动距离，乘以一个角速度和时间间隔，得到新的yaw角度
		//yaw = clamp<float>(yaw, -MAX_YAW_RAD, MAX_YAW_RAD);
		std::cout << "现在的旋转角度yaw：" << yaw << std::endl;
	}
};
