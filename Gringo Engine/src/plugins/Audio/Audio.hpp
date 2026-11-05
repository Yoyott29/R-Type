/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** Audio
*/

#pragma once

#include "Application.hpp"

class Audio : public Plugin<Audio>
{
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;

  protected:
  private:
};
