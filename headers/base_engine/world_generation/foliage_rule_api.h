#pragma once

#include <common.h>
#include <string_view>
#include <glm/glm.hpp>

namespace chunk_foliage
{
struct instanced_foliage_rule
{
  /*
  useful to the rule generator script path and name of object
  feel free to change to something else later that we use actual game objects not just meshes
  */
  std::string_view path, name;

  /*
  these will be able to be set by the script but have sane default values
  random rotation refers to object local y axis nothing else 360 means it can be rotated free (all 360 degrees WOW!)
  random scale amplitude the range of randomness generated in scale negative means only gets scaled smaller than 100% positive
  means other way around. seems weird because you might think "what if i want both" but that also works if you think about it (jst scale
  the entire model up or down to change limits)
  */
  f32 random_rotation_amplitude = 360.f, random_scale_amplitude = -0.05f;

  /*
  when the generator found a viable spot in the first generation iteration
  it uses this chance to figure out if it actually should spawn a thingy here
  */
  f32 spawn_chance = 1.f;

  /*
  the minimum distance between the instanced objects to prevent them spawning inside of each other
  since an ellipse can be defined as two radii this is vec2 not a vec3 even though it works for 3 dimensions
  */
  glm::vec2 min_dist{};
  /*
   this sets the minimum distance exactly to bounding box size of the object
   */
  bool min_dist_is_bbox = true;

  /*
  for figuring out if the currect foliage object should generate at this normal value
   */
  struct generation_direction_rule
  {
    /*
    this is the preferred direction of the object for example trees and grass want to mainly grow only upwards
    */
    glm::vec3 preferred_direction{0.f, 1.f, 0.f};

    /*
    this is how much the currently checked spots normal vector can vary from the preferred direction
    for example trees grass should also be able to spawn on slight hills or uneven ground not just at
    the places that perfectly point to {0,1,0}
    */
    glm::vec3 maximum_direction_diff{0.1f, 0.1f, 0.1f};
  };

  std::vector<generation_direction_rule> direction_rules{};

  /*
  flags to be set by the generator script
  set_to_ground: rotate the instance model so that it sits on the ground properly
  others are self explanatory
  */
  bool set_to_ground = true, scale_randomly = false, rotate_randomly = true;
};

static const std::unordered_map<usize, instanced_foliage_rule> foliage_rule_objects;

} // namespace chunk_foliage