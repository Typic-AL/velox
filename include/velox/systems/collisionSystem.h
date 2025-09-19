#include "../components/collider.h"
#include "velox/registry.h"

namespace vl {

bool checkLayer(Collider &col1, Collider &col2);
void sweepAndPrune(Registry &reg);

}
