#include "../components/collider.h"
#include "velox/registry.h"
#include <set>

namespace vl {

bool checkLayer(Collider &col1, Collider &col2);
void sweepAndPrune(Registry &reg);

inline std::set<std::pair<Entity, Entity>> currentFrameCols;
inline std::set<std::pair<Entity, Entity>> prevFrameCols;

}
