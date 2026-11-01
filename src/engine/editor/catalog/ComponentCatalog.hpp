/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** ComponentCatalog
*/

#ifndef COMPONENTCATALOG_HPP_
#define COMPONENTCATALOG_HPP_

#include "ComponentDescriptor.hpp"
#include <memory>
#include <vector>

class ComponentCatalog
{
  public:
    ComponentCatalog();

    const std::vector<ComponentDescriptor> &getDescriptors() const { return _descriptors; }

  private:
    std::vector<ComponentDescriptor> _descriptors;

    // Kept alive until the end of the program, the type is erased to hide IPlugin from this header
    std::vector<std::shared_ptr<void>> _loadedPlugins;
};

#endif /* !COMPONENTCATALOG_HPP_ */
