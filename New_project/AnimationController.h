#pragma once
#include"Animation.h"
enum class States
{
	Idle,
	Run,
	Attack,
	Death,
	Roar
};
class AnimationController  //每一个渲染对象都有一个动画控制器，用来控制当前动画的播放状态，因为两只羊的动画可能是不一样的
{
	  //这个指针用来指向当前渲染对象的动画实例数据，并控制播放状态
	
public:
	bool animationisfinish = false;
	AnimationInstance* anim;
	States currentState = States::Idle;  //默认依赖的状态是Idle，这个变量需要从外部来修改，通过自己的方法
	void init(AnimationInstance* _anim)
	{
		anim = _anim;
	}
	void update(float dt) //这个必须也是每帧调用，因为需要传进时间增量来更新动画并且还要判断动画是否播放完毕
	{
		std::string animName;
		switch (currentState)
		{
		case States::Idle:
			animName = "idle";
			break;
		case States::Run:
			animName = "run";
			break;
		case States::Attack:
			animName = "attack";
			break;
		case States::Death:
			animName = "death";
			break;
		case States::Roar:
			animName = "roar";
			break;
		default:
			animName = "idle2";
			break;
		}
		anim->update(animName, dt);
		if (anim->animationFinished() == true)
		{
			if (currentState != States::Death)  //如果不是死亡动画，重新接入idle，如果是死亡动画，直接改bool，下次不用再更新了
			{
				currentState = States::Idle;  //当设置为死亡状态并且重新恢复到默认状态表示死亡动画播放完毕，可以死亡了
				anim->resetAnimationTime();  //重新设置动画的播放时间
			}
			else
			{
				animationisfinish = true;//用这个来判断死亡
			}
		}	
	}
	void setState(States newState)
	{
		if (newState != currentState)  //必须确保是一个新的状态才切换，不然一直卡在开始
		{
			currentState = newState;
			anim->resetAnimationTime();  //切换状态时重置动画时间,但是只有update才会实际触发动画效果
		}
	}
};