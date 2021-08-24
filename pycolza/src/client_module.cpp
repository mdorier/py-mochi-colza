#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <mpi4py/mpi4py.h>
#include <colza/Client.hpp>

namespace py11 = pybind11;
namespace np = py11;
using namespace pybind11::literals;
using namespace colza;

struct margo_instance;
typedef struct margo_instance* margo_instance_id;
typedef py11::capsule pymargo_instance_id;

#define MID2CAPSULE(__mid)   py11::capsule((void*)(__mid), "margo_instance_id", nullptr)
#define CAPSULE2MID(__caps)  (margo_instance_id)(__caps)

class ColzaCommunicator : public ClientCommunicator {

    MPI_Comm comm;

    public:

    ColzaCommunicator(py11::handle src) {
        PyObject *py_src = src.ptr();
        // Check that we have been passed an mpi4py communicator
        if (PyObject_TypeCheck(py_src, &PyMPIComm_Type)) {
            // Convert to regular MPI communicator
            comm = *PyMPIComm_Get(py_src);
        } else {
            throw std::runtime_error("ColzaCommunicator should be initialized with an mpi4py communicator");
        }
    }

    ~ColzaCommunicator() = default;

    int size() const override {
        int size;
        MPI_Comm_size(comm, &size);
        return size;
    }

    int rank() const override {
        int rank;
        MPI_Comm_rank(comm, &rank);
        return rank;
    }

    void barrier() const override {
        MPI_Barrier(comm);
    }

    void bcast(void* buffer, int bytes, int root) const {
        MPI_Bcast(buffer, bytes, MPI_BYTE, root, comm);
    }
};

PYBIND11_MODULE(_colza_client, m) {
    py11::module::import("numpy");
    import_mpi4py();

    m.doc() = "Colza client C++ extension";
    py11::class_<ColzaCommunicator>(m, "ColzaCommunicator")
        .def(py11::init<py11::handle>());

    py11::enum_<Type>(m, "Type")
#define X(t) .value(#t, Type::t)
        COLZA_TYPES
#undef X
    ;

    py11::class_<Client>(m, "ColzaClient")
        .def(py11::init<pymargo_instance_id>())
        .def("make_pipeline_handle",
             &Client::makePipelineHandle,
             "Create a pipeline handle.",
             "address"_a,
             "provider_id"_a,
             "name"_a,
             "check"_a=true)
        .def("make_distributed_pipeline_handle",
             &Client::makeDistributedPipelineHandle,
             "Create a distributed pipeline handle.",
             "comm"_a,
             "ssg_file"_a,
             "provider_id"_a,
             "name"_a,
             "check"_a=true)
    ;
    py11::class_<PipelineHandle>(m, "ColzaPipelineHandle")
        .def("start", &PipelineHandle::start, "Start an iteration.", "iteration"_a)
        .def("execute", [](const PipelineHandle& p, uint64_t iteration, bool autoCleanup) {
                int32_t result;
                p.execute(iteration, &result, autoCleanup, nullptr);
                return result;
            }, "Execute an iteration.",
            "iteration"_a, "auto_cleanup"_a=false)
        .def("cleanup", [](const PipelineHandle& p, uint64_t iteration) {
                int32_t result;
                p.cleanup(iteration, &result);
                return result;
            }, "Cleanup an iteration.",
            "iteration"_a)
        .def("stage", [](const PipelineHandle& p, uint64_t iteration,
                         uint64_t block_id, const std::vector<size_t>& dimensions,
                         const std::vector<int64_t>& offsets, const Type& type,
                         const np::array& data) {
                // TODO
            }, "Send data to the pipeline.",
            "iteration"_a, "block_id"_a,
            "dimensions"_a, "offsets"_a,
            "type"_a, "data"_a);
        ;
    py11::class_<DistributedPipelineHandle>(m, "ColzaDistributedPipelineHandle")
        .def("start", &DistributedPipelineHandle::start, "Start an iteration.", "iteration"_a)
        .def("execute", [](const DistributedPipelineHandle& p, uint64_t iteration, bool autoCleanup) {
                int32_t result;
                p.execute(iteration, &result, autoCleanup, nullptr);
                return result;
            }, "Execute an iteration.",
            "iteration"_a, "auto_cleanup"_a=false)
        .def("cleanup", [](const DistributedPipelineHandle& p, uint64_t iteration) {
                int32_t result;
                p.cleanup(iteration, &result);
                return result;
            }, "Cleanup an iteration.",
            "iteration"_a)
        .def("stage", [](const DistributedPipelineHandle& p, uint64_t iteration,
                         uint64_t block_id, const std::vector<size_t>& dimensions,
                         const std::vector<int64_t>& offsets, const Type& type,
                         const np::array& data) {
                // TODO
            }, "Send data to the pipeline.",
            "iteration"_a, "block_id"_a,
            "dimensions"_a, "offsets"_a,
            "type"_a, "data"_a);
        ;
}
