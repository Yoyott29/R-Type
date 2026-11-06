/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** AnimationSystem
*/

#pragma once
#include "AnimationLibrary.hpp"
#include "Animator.hpp"
#include "Scene.hpp"
#include "Sprite.hpp"
#include "System.hpp"
#include <string>

// Runs in FixedUpdate: advances every Animator's animation and shows its current frame on the
// Sprite (texture + region). Playing a name with no name.anim file throws, so a typo is not silent
class AnimationSystem : public System
{
  public:
    void update(Scene &scene) override;

    // Switches the animator to <animation>.anim (next to its starting animation), from frame 0.
    // Already playing it: nothing changes, so game code can call it every frame
    //   AnimationSystem::play(scene.getComponent<Animator>(entity), "run");
    // Defined here: game scripts only get the plugin's headers, they do not link 2D.so
    static void play(Animator &animator, const std::string &animation)
    {
        if (animation == animator.current)
            return;
        animator.current = animation;
        animator.frame = 0;
        animator.elapsed = 0.0f;
        animator.finished = false;
    }

  private:
    // Moves the animator forward by dt seconds in its clip
    void advance(Animator &animator, const AnimationClip &clip, float dt) const;
    // Points the sprite at the animator's current frame
    void showFrame(Sprite &sprite, const Animator &animator, const AnimationClip &clip) const;
};
