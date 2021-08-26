/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __TEST_BACKEND_HPP
#define __TEST_BACKEND_HPP

#include <thallium.hpp>
#include <colza/Backend.hpp>

using json = nlohmann::json;
namespace tl = thallium;

struct DataBlock {

    std::vector<char>    data;
    std::vector<size_t>  dimensions;
    std::vector<int64_t> offsets;
    colza::Type          type;

    DataBlock()                            = default;
    DataBlock(DataBlock&&)                 = default;
    DataBlock(const DataBlock&)            = default;
    DataBlock& operator=(DataBlock&&)      = default;
    DataBlock& operator=(const DataBlock&) = default;
    ~DataBlock()                           = default;

};

/**
 * Test implementation of an colza Backend.
 */
class TestPipeline : public colza::Backend {

    protected:

    tl::engine     m_engine;
    ssg_group_id_t m_gid;
    json           m_config;
    std::map<uint64_t,         // iteration
        std::map<std::string,  // dataset name
            std::map<uint64_t, // block id
                     DataBlock
                    >
                >
            > m_datasets;
    tl::mutex m_datasets_mtx;

    public:

    /**
     * @brief Constructor.
     */
    TestPipeline(const colza::PipelineFactoryArgs& args)
    : m_engine(args.engine)
    , m_gid(args.gid)
    , m_config(args.config) {}

    /**
     * @brief Move-constructor.
     */
    TestPipeline(TestPipeline&&) = delete;

    /**
     * @brief Copy-constructor.
     */
    TestPipeline(const TestPipeline&) = delete;

    /**
     * @brief Move-assignment operator.
     */
    TestPipeline& operator=(TestPipeline&&) = delete;

    /**
     * @brief Copy-assignment operator.
     */
    TestPipeline& operator=(const TestPipeline&) = delete;

    /**
     * @brief Destructor.
     */
    virtual ~TestPipeline() = default;

    /**
     * @brief Update the array of Mona addresses associated with
     * the SSG group.
     *
     * @param mona Mona instance.
     * @param addresses Array of Mona addresses.
     */
    void updateMonaAddresses(
            mona_instance_t mona,
            const std::vector<na_addr_t>& addresses) override;

    /**
     * @brief Tells the pipeline that the given iteration is starting.
     * This function should be called before stage/execute/cleanup can
     * be called.
     *
     * @param iteration Iteration
     *
     * @return a RequestResult containing an error code.
     */
    colza::RequestResult<int32_t> start(uint64_t iteration) override;

    /**
     * @brief Tells the pipeline that the given iteration is aborted.
     * This function is used automatically when there is a mismatch
     * between the client's view of the group and the group itself.
     *
     * @param iteration Iteration
     */
    void abort(uint64_t iteration) override;

    /**
     * @brief Stage some data.
     */
    colza::RequestResult<int32_t> stage(
            const std::string& sender_addr,
            const std::string& dataset_name,
            uint64_t iteration,
            uint64_t block_id,
            const std::vector<size_t>& dimensions,
            const std::vector<int64_t>& offsets,
            const colza::Type& type,
            const thallium::bulk& data) override;

    /**
     * @brief The execute method in this backend is not doing anything.
     */
    colza::RequestResult<int32_t> execute(
            uint64_t iteration) override;

    /**
     * @brief Erase all the data blocks associated with a given iteration.
     */
    colza::RequestResult<int32_t> cleanup(
            uint64_t iteration) override;

    /**
     * @brief Destroys the underlying pipeline.
     *
     * @return a RequestResult<int32_t> instance indicating
     * whether the database was successfully destroyed.
     */
    colza::RequestResult<int32_t> destroy() override;

    /**
     * @brief Static factory function used by the PipelineFactory to
     * create a TestPipeline.
     *
     * @param args arguments used for creating the pipeline.
     *
     * @return a unique_ptr to a pipeline
     */
    static std::unique_ptr<colza::Backend> create(const colza::PipelineFactoryArgs& args);
};

#endif
