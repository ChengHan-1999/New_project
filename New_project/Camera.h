#pragma once
#include"CGBase.h"
#include<iostream>
#include"GamesEngineeringBase.h"
#include"Window.h"
class Camera
{
public:
	Vec3 camaraposition{ 0.f,8.f,-10.f };  //这是类初始化位置
	float yaw = 0.0f;  //我这个应该是绕y轴旋转角度把
	float cameraspeed = 1.f;
	float cameraanglespeed = 1.f;  //这个就是相机的映射比例
	Matrix getViewMatrix(const Vec3& heroposition)
	{
		Vec3 forward(
			sinf(yaw),
			0,
			cosf(yaw)
		);  //这个用来表示相机旋转时自己位置的偏移量

		return Matrix::Lookat(camaraposition, camaraposition + forward , Vec3(0, 1, 0));  //这个就是初始的视图矩阵
		//Matrix::Lookat(camaraposition, heroposition + forward, Vec3(0, 1, 0));  //这个就是初始的视图矩阵
	}
	void updateCameraPosition(const Window& canvas, float deltaTime,float dx)
	{
		Vec3 forward(  //当yaw为0的时候，forward就是指向正z轴方向的  ，，所以forward就是一个表示前进方向的单位向量
			sinf(yaw),
			0,
			cosf(yaw)
		);  //这个用来表示相机旋转时自己位置的偏移量
		Vec3 right = Vec3(0, 1, 0).Cross(forward).normalize();  //右方向向量,为什么这么写是因为d3d12是左手坐标系，摄像机的forward方向是正z轴方向
		if (canvas.keys['W'])  //前进
		{
			camaraposition += forward * cameraspeed * deltaTime;  //前进的方向是forward方向  * 速度 * 帧间隔时间
		}
		else if (canvas.keys['S'])  //后退
		{
			camaraposition -= forward * cameraspeed * deltaTime;
		}
		else if (canvas.keys['A'])  //左移
		{
			camaraposition -= right * cameraspeed * deltaTime;
		}
		else if (canvas.keys['D'])  //右移
		{
			camaraposition += right * cameraspeed * deltaTime;
		}
		//在相机中我们需要人为规定一个映射比例，来表示在屏幕上鼠标移动的像素距离对应于相机旋转的角度变化
		
		yaw += dx * cameraanglespeed * deltaTime;  //这里的dx是鼠标在x轴上的移动距离，乘以一个角速度和时间间隔，得到新的yaw角度
	}
};
