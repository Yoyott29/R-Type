/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** AnimationSystem
*/

#include "AnimationSystem.hpp"
#include "Time.hpp"
#include <filesystem>

void AnimationSystem::update(Scene &scene)
{
    AnimationLibrary &library = scene.getResource<AnimationLibrary>();

    for (Entity entity : _entities) {
        Animator &animator = scene.getComponent<Animator>(entity);
        if (animator.path.empty())
            continue;
        const std::filesystem::path start(animator.path);
        if (animator.current.empty())
            play(animator, start.stem().string());
        // play("run"): run.anim, next to the starting animation
        const std::filesystem::path file =
            start.parent_path() / (animator.current + start.extension().string());
        const AnimationClip &clip = library.get(scene, file.string());
        advance(animator, clip, FIXED_DELTA_TIME);
        showFrame(scene.getComponent<Sprite>(entity), animator, clip);
    }
}

void AnimationSystem::advance(Animator &animator, const AnimationClip &clip, float dt) const
{
    const float rate = clip.fps * animator.speed;
    if (animator.finished || rate <= 0.0f)
        return;
    animator.elapsed += dt;
    const float frameDuration = 1.0f / rate;
    while (animator.elapsed >= frameDuration) {
        animator.elapsed -= frameDuration;
        if (animator.frame + 1 < clip.frames) {
            ++animator.frame;
        } else if (clip.loop) {
            animator.frame = 0;
        } else {
            animator.finished = true;
            animator.elapsed = 0.0f;
            return;
        }
    }
}

void AnimationSystem::showFrame(Sprite &sprite, const Animator &animator,
                                const AnimationClip &clip) const
{
    const int cell = clip.start + animator.frame;
    sprite.texturePath = clip.texturePath;
    sprite.region = {static_cast<float>(cell % clip.columns) * clip.frameWidth,
                     static_cast<float>(cell / clip.columns) * clip.frameHeight, clip.frameWidth,
                     clip.frameHeight};
}
