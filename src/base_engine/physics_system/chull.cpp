#include <deque>
#define ENABLE_VHACD_IMPLEMENTATION 1
#include <vhacd/VHACD.h>

#include <cmath>
#include <exception>
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <quickhull/QuickHull.hpp>
#include <vector>

#include <base_engine/physics_system/chull.h>
#include <base_engine/physics_system/mesh_collider.h>
#include <base_engine/debug/debug_overlay.h>
#include <base_engine/physics_system/bbox.h>
#include <vhacd/wavefront.h>
#include <qms/Simplify.h>

#ifdef min
#undef min
#else
#ifdef max
#undef max
#endif
#endif

// stupid logging thing the library wants
class Logging : public VHACD::IVHACD::IUserCallback, public VHACD::IVHACD::IUserLogger
{
public:
  Logging(void) {}

  ~Logging(void) { flushMessages(); }

  // Be aware that if you are running V-HACD asynchronously (in a background thread) this callback will come from
  // a different thread. So if your print/logging code isn't thread safe, take that into account.
  virtual void
  Update(const double overallProgress, const double stageProgress, const char* const stage, const char* operation) final
  {
    char scratch[512];
    snprintf(scratch, sizeof(scratch), "[%-40s] : %0.0f%% : %0.0f%% : %s", stage, overallProgress, stageProgress, operation);

    if (strcmp(stage, mCurrentStage.c_str()) == 0)
    {
    }
    else
    {
      mCurrentStage = std::string(stage);
    }
    mLastLen = (uint32_t)strlen(scratch);
    LOG(INFO) << std::string{scratch, mLastLen};
  }

  // This is an optional user callback which is only called when running V-HACD asynchronously.
  // This is a callback performed to notify the user that the
  // convex decomposition background process is completed. This call back will occur from
  // a different thread so the user should take that into account.
  virtual void
  NotifyVHACDComplete(void)
  {
    Log("VHACD::Complete");
  }

  virtual void
  Log(const char* const msg) final
  {
    mLogMessages.push_back(std::string(msg));
  }

  void
  flushMessages(void)
  {
    if (!mLogMessages.empty())
    {
      LOG(INFO) << "";
      for (auto& i : mLogMessages)
      {
        LOG(INFO) << i;
      }
      mLogMessages.clear();
    }
  }

  uint32_t mLastLen{0};
  std::string mCurrentStage;
  std::vector<std::string> mLogMessages;
};

u0
convex_hull_t::load(const mesh_t& _mesh)
{
  for (const auto& point : _mesh.vertices)
  {
    points.push_back(point.position);
  }
  for (const auto& index : _mesh.indices)
  {
    indices.push_back(index);
  }
}

u0
convex_hull_t::from_mesh(const mesh_t& _mesh)
{
  static constexpr auto to_glm = [](const quickhull::Vector3<f32>& _v) { return glm::vec3{_v.x, _v.y, _v.z}; };
  static constexpr auto to_qh  = [](const glm::vec3& _v) { return quickhull::Vector3<f32>{_v.x, _v.y, _v.z}; };

  std::vector<glm::vec3> vertex_raw(_mesh.vertices.size());

  for (usize i = 0; i != _mesh.vertices.size(); ++i)
  {
    vertex_raw[i] = _mesh.vertices[i].position;
  }

  quickhull::QuickHull<f32> qh{};
  std::vector<quickhull::Vector3<f32>> points_raw(vertex_raw.size());

  for (auto& [i, ite] : enumerate(vertex_raw))
  {
    points_raw[i] = to_qh(ite);
  }

  auto hull = qh.getConvexHull(points_raw, true, false);

  for (const auto& vec : hull.getVertexBuffer())
  {
    points.push_back(to_glm(vec));
    center += to_glm(vec);
  }

  for (const auto& id : hull.getIndexBuffer())
  {
    indices.push_back(id);
  }

  center /= points.size();
}

// distance based reduce
std::vector<glm::vec3>
convex_hull_t::reduce(const std::vector<glm::vec3>& _vertices, double _min_distance)
{
  // find sensible value
  if (_min_distance == 0.f)
  {
  }

  std::vector<glm::vec3> simplified{};
  int size = _vertices.size();

  glm::vec3 ref = _vertices.at(0);
  simplified.push_back(ref);
  for (int i = 1; i < size; i++)
  {
    glm::vec3 v0 = _vertices.at(i);
    if (glm::length(ref - v0) > _min_distance)
    {
      ref = v0;
      simplified.push_back(v0);
    }
  }
  return simplified;
}

bool
convex_hull_t::to_submeshes()
{
  static constexpr u32 cols[]{0xff0000ff, 0xffff00ff, 0xff00ffff, 0x00ff00ff, 0x00ffffff, 0x0000ffff, 0xff0f0fff, 0xfff0f0ff, 0xf0f0f0ff};

  if (!simplified_chulls.empty())
  {
    for (const auto& submesh : simplified_chulls)
    {
      usize i = 0;
      for (const auto& point : submesh)
      {
        glPointSize(5);
        debug_overlay_t::draw_point(point, cols[i % 9], true);
        glPointSize(1);
        ++i;
      }
    }
    return true;
  }

  WavefrontObj new_obj;
  new_obj.loadObj("../data/duskwoodchapel2.obj");

  Simplify::load_obj("../data/duskwoodchapel.obj");
  Simplify::simplify_mesh(Simplify::triangles.size(), 4.f, true);
  Simplify::write_obj("../data/duskwoodchapel2.obj");

  Logging logging;
  VHACD::IVHACD::Parameters p;

  p.m_callback = &logging;
  p.m_logger   = &logging;

  p.m_fillMode            = VHACD::FillMode::RAYCAST_FILL;
  p.m_maxNumVerticesPerCH = 4;
  p.m_maxConvexHulls      = 50;
  p.m_findBestPlane       = false;
  p.m_shrinkWrap          = false;

  VHACD::IVHACD* iface = VHACD::CreateVHACD();

  iface->Compute(new_obj.mVertices, new_obj.mVertexCount, new_obj.mIndices, new_obj.mTriCount, p);

  LOG(INFO) << "finished generating!";

  for (usize i = 0; i < iface->GetNConvexHulls(); i++)
  {
    LOG(INFO) << "found " << iface->GetNConvexHulls() << " convex hulls!";
    if (iface->GetNConvexHulls() == 0)
    {
      LOG(INFO) << "error subdividing mesh!";
      glfwTerminate();
      std::exit(-1);
    }
    VHACD::IVHACD::ConvexHull ch;

    iface->GetConvexHull(i, ch);

    std::vector<glm::vec3> hull_p{};

    for (auto& point : ch.m_points)
    {
      hull_p.push_back({point.mX, point.mY, point.mZ});
      LOG(INFO) << "chull v: x:" << point.mX << " y:" << point.mY << " z:" << point.mZ;
    }

    simplified_chulls.push_back(hull_p);
  }

  iface->Release();
  return true;
}

// this should be done to collision mesh not chull
bool
convex_hull_t::simplify()
{
  if (indices.size() < 3 || points.size() < 3)
  {
    LOG(INFO) << "tried to simplify invalid chull!";
    return false;
  }

  std::vector<std::pair<usize, glm::vec3>> face_points(indices.size());

  for (const auto [i, index] : enumerate(indices))
  {
    face_points[i] = {index, points[index]};
  }

  /* choose first points */
  auto& first_point  = points[indices[0]];
  auto& second_point = points[indices[1]];
  auto& third_point  = points[indices[2]];

  static constexpr auto same_side = [](const glm::vec3& point, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
  {
    glm::vec3 v_point = point - p1;
    glm::vec3 normal  = glm::cross(p2 - p1, p3 - p1);
    normal            = glm::normalize(normal);

    f32 dot_point = glm::dot(normal, v_point);
    bool on_side  = dot_point > 0;
    return on_side;
  };

  static constexpr auto distance_plane = [](const glm::vec3& point, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
  {
    glm::vec3 normal = glm::cross(p2 - p1, p3 - p1);
    normal           = glm::normalize(normal);
    f32 distance     = glm::dot(normal, point - p1);
    return std::fabsf(distance);
  };

  auto simplified = reduce(points, 30.0f);

  for (const auto& point : simplified)
  {
    debug_overlay_t::draw_point(point, 0xffffffff, true);
  }

  return true;
}