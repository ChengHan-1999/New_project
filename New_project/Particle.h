#pragma once
#include"CGBase.h"
#include <vector>
#include <random>
class ParticleSystem
{
public:
	struct Particle  //这个结构体用来表示单个粒子
    {
        Vec3 pos;
        Vec3 vel;
        float life;
        float size;
    };

    std::vector<Particle> particles;

    void emit(const Vec3& pos)
    {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
		std::uniform_real_distribution<float> dist(2.0f, 4.0f);
        for (int i = 0; i < 50; ++i)  //还有一个问题，我这里拿到了每一个点在空间中的位置，所以我要传给VSshader直接计算吗
        {
            Particle p;
            p.pos = pos;
            p.vel = Vec3(
                dis(gen),
                dist(gen),
                dis(gen)
            );  //这样生成随机的粒子速度是对的吗
            p.life = 1.0f;
            p.size = 0.2f;
            particles.push_back(p);
        }
    }

    void update(float dt)  //这里要传时间进来做递增
    {
        Vec3 gravity(0, -9.8f, 0);

        for (auto& p : particles)
        {
            p.vel += gravity * dt;
            p.pos += p.vel * dt;
            p.life -= dt;
        }

        // 删除死粒子
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [](auto& p) { return p.life <= 0; }),
			particles.end()  //每次更新都把死掉的粒子删掉 ，，这个vector的用法要重新复习一下
        );
    }
};
