#include <pybind11/stl.h>
#include <colza/Admin.hpp>

namespace py11 = pybind11;
using namespace pybind11::literals;
using namespace colza;

struct margo_instance;
typedef struct margo_instance* margo_instance_id;
typedef py11::capsule pymargo_instance_id;

#define MID2CAPSULE(__mid)   py11::capsule((void*)(__mid), "margo_instance_id", nullptr)
#define CAPSULE2MID(__caps)  (margo_instance_id)(__caps)

PYBIND11_MODULE(_colza_admin, m) {
    m.doc() = "Colza admin C++ extension";

    py11::class_<Admin>(m, "ColzaAdmin")
        .def(py11::init<pymargo_instance_id>())
        .def("create_pipeline", [](const Admin& admin,
                                   const std::string& address,
                                   uint16_t provider_id,
                                   const std::string& name,
                                   const std::string& type,
                                   const std::string& config,
                                   const std::string& library,
                                   const std::string& token) {
                    admin.createPipeline(address, provider_id, name, type, config, library, token);
                },
                "address"_a,
                "provider_id"_a,
                "name"_a,
                "type"_a,
                "config"_a,
                "library"_a=std::string(""),
                "security_token"_a=std::string(""))
        .def("create_distributed_pipeline", [](const Admin& admin,
                                               const std::string& ssg_file,
                                               uint16_t provider_id,
                                               const std::string& name,
                                               const std::string& type,
                                               const std::string& config,
                                               const std::string& library,
                                               const std::string& token) {
                    admin.createDistributedPipeline(ssg_file, provider_id, name, type, config, library, token);
                },
                "ssg_file"_a,
                "provider_id"_a,
                "name"_a,
                "type"_a,
                "config"_a,
                "library"_a=std::string(""),
                "security_token"_a=std::string(""))
        .def("destroy_pipeline", [](const Admin& admin,
                                    const std::string& address,
                                    uint16_t provider_id,
                                    const std::string& name,
                                    const std::string& token) {
                    admin.destroyPipeline(address, provider_id, name, token);
                },
                "address"_a,
                "provider_id"_a,
                "name"_a,
                "security_token"_a=std::string(""))
        .def("destroy_distributed_pipeline", [](const Admin& admin,
                                                const std::string& ssg_file,
                                                uint16_t provider_id,
                                                const std::string& name,
                                                const std::string& token) {
                    admin.destroyDistributedPipeline(ssg_file, provider_id, name, token);
                },
                "ssg_file"_a,
                "provider_id"_a,
                "name"_a,
                "security_token"_a=std::string(""))
        ;
}
