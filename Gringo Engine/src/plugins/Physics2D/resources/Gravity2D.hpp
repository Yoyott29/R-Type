/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Gravity2D
*/

#pragma once

// Scene resource: the acceleration (pixels per second squared) pulling every RigidBody2D, read
// from the Physics2D project settings "GravityX" and "GravityY". y goes down the screen
struct Gravity2D {
    float x = 0.0f;
    float y = 980.0f;
};
