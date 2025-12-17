#pragma once
#include"CGBase.h"
#include<iostream>
struct AABB
{
	Vec3 min;  //这两个分别是包围盒的最小点和最大点，默认是对焦先
	Vec3 max;
};
static AABB WorldBox(const Vec3& WorldPos)
{
	AABB localBox{ Vec3(-2,0,-2),Vec3(2,3,2) };  //我直接确定写死一个每个物体的局部坐标系下的包围盒

	return AABB{ localBox.min + WorldPos,localBox.max + WorldPos };  //转化为世界坐标系下的包围盒
}
static bool AABBvsAABB(const Vec3& obj1, const Vec3& obj2)  //这个应该是每帧检测世界坐标系下的包围和碰撞
{
	AABB box1 = WorldBox(obj1);
	AABB box2 = WorldBox(obj2);  //直接把两个位置传进来求各自世界坐标系的包围盒
	if (box1.max.x < box2.min.x || box1.min.x > box2.max.x || box1.max.y < box2.min.y || box1.min.y > box2.max.y || box1.max.z < box2.min.z || box1.min.z > box2.max.z) return false;
	else {
		return true;  //所有轴向上都分离了说明没有碰撞，否则就碰撞了
	}

}
void ResolveAABB( const Vec3& dinosaurpos,Vec3& collideobjposition) //这个函数是用来解决两个AABB碰撞后的穿透问题的，把被碰撞的另一个顶出去
{
    AABB dinosauraabbworld = WorldBox(dinosaurpos);  //也是直接传位置就行了
    AABB Collideobjaabb = WorldBox(collideobjposition);  //被撞物体世界和就是通过被撞物体的坐标直接推出来的
    float overlapX1 = dinosauraabbworld.max.x - Collideobjaabb.min.x;
    float overlapX2 = Collideobjaabb.max.x - dinosauraabbworld.min.x;
    float overlapX = std::min(overlapX1, overlapX2);

    float overlapZ1 = dinosauraabbworld.max.z - Collideobjaabb.min.z;
    float overlapZ2 = Collideobjaabb.max.z - dinosauraabbworld.min.z;
    float overlapZ = std::min(overlapZ1, overlapZ2);

    // 选最小穿透方向推出
    if (overlapX < overlapZ)
    {
        collideobjposition.x += (Collideobjaabb.min.x < dinosauraabbworld.min.x) ? -overlapX : overlapX;
    }
    else
    {
        collideobjposition.z += (Collideobjaabb.min.z < dinosauraabbworld.min.z) ? -overlapZ : overlapZ;
    }
}