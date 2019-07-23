#include "tsl/algorithm/reduction.hpp"

namespace tsl {

size_t remove_edges(tmesh& mesh, double percent) {
    assert(percent <= 100.0);
    assert(percent > 0);

    uint32_t deleted = 0;
    double count = 1.0;
    auto delete_every_n_edge = static_cast<size_t>(100.0 / percent);
    for (const auto& edge: mesh.get_edges()) {
        if (count > delete_every_n_edge) {
            count -= delete_every_n_edge;
            if (mesh.remove_edge(edge)) {
                deleted += 1;
            }
        } else {
            count += 1.0;
        }
    }

    return deleted;
}

}
