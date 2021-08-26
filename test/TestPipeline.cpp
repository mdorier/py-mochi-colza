/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "TestPipeline.hpp"
#include <spdlog/spdlog.h>

COLZA_REGISTER_BACKEND(test, TestPipeline);

void TestPipeline::updateMonaAddresses(
        mona_instance_t mona,
        const std::vector<na_addr_t>& addresses) {
    spdlog::trace("Mona addresses have been updated, group size is now {}", addresses.size());
    (void)addresses;
    (void)mona;
}

colza::RequestResult<int32_t> TestPipeline::start(uint64_t iteration) {
    spdlog::trace("Iteration {} starting", iteration);
    colza::RequestResult<int32_t> result;
    result.success() = true;
    result.value() = 0;
    return result;
}

void TestPipeline::abort(uint64_t iteration) {
    spdlog::trace("Iteration {} aborted", iteration);
}

colza::RequestResult<int32_t> TestPipeline::execute(
        uint64_t iteration) {
    (void)iteration;
    spdlog::trace("Iteration {} executing", iteration);
    auto result = colza::RequestResult<int32_t>();
    result.value() = 0;
    return result;
}

colza::RequestResult<int32_t> TestPipeline::cleanup(
        uint64_t iteration) {
    std::lock_guard<tl::mutex> g(m_datasets_mtx);
    spdlog::trace("Iteration {} cleaned up", iteration);
    m_datasets.erase(iteration);
    auto result = colza::RequestResult<int32_t>();
    result.value() = 0;
    return result;
}

colza::RequestResult<int32_t> TestPipeline::stage(
        const std::string& sender_addr,
        const std::string& dataset_name,
        uint64_t iteration,
        uint64_t block_id,
        const std::vector<size_t>& dimensions,
        const std::vector<int64_t>& offsets,
        const colza::Type& type,
        const thallium::bulk& data) {
    colza::RequestResult<int32_t> result;
    spdlog::trace("Block {} for dataset {} sent by {} at iteration {}",
            block_id, dataset_name, sender_addr, iteration);
    result.value() = 0;
    {
        std::lock_guard<tl::mutex> g(m_datasets_mtx);
        if(m_datasets.count(iteration) != 0
        && m_datasets[iteration].count(dataset_name) != 0
        && m_datasets[iteration][dataset_name].count(block_id) != 0) {
            result.error() = "Block already exists for provided iteration, name, and id";
            result.success() = false;
            return result;
        }
    }
    DataBlock block;
    block.dimensions = dimensions;
    block.offsets    = offsets;
    block.type       = type;
    block.data.resize(data.size());

    try {
        std::vector<std::pair<void*, size_t>> segments = {
            std::make_pair<void*, size_t>(block.data.data(), block.data.size())
        };
        auto local_bulk = m_engine.expose(segments, tl::bulk_mode::write_only);
        auto origin_ep = m_engine.lookup(sender_addr);
        data.on(origin_ep) >> local_bulk;
    } catch(const std::exception& ex) {
        result.success() = false;
        result.error() = ex.what();
    }

    if(result.success()) {
        std::lock_guard<tl::mutex> g(m_datasets_mtx);
        m_datasets[iteration][dataset_name][block_id] = std::move(block);
    }
    return result;
}

colza::RequestResult<int32_t> TestPipeline::destroy() {
    colza::RequestResult<int32_t> result;
    result.value() = true;
    return result;
}

std::unique_ptr<colza::Backend> TestPipeline::create(const colza::PipelineFactoryArgs& args) {
    return std::unique_ptr<colza::Backend>(new TestPipeline(args));
}
