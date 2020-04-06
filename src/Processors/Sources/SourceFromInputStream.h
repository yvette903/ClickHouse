#pragma once
#include <Processors/Sources/SourceWithProgress.h>
#include <Processors/RowsBeforeLimitCounter.h>

namespace DB
{

class IBlockInputStream;
using BlockInputStreamPtr = std::shared_ptr<IBlockInputStream>;

/// Wrapper for IBlockInputStream which implements ISourceWithProgress.
class SourceFromInputStream : public ISourceWithProgress
{
public:
    explicit SourceFromInputStream(BlockInputStreamPtr stream_, bool force_add_aggregating_info_ = false);
    String getName() const override { return "SourceFromInputStream"; }

    Status prepare() override;
    void work() override;

    Chunk generate() override;

    BlockInputStreamPtr & getStream() { return stream; }

    void addTotalsPort();

    void setRowsBeforeLimitCounter(RowsBeforeLimitCounterPtr counter) { rows_before_limit.swap(counter); }

    /// Implementation for methods from ISourceWithProgress.
    void setLimits(const LocalLimits & limits_) final { stream->setLimits(limits_); }
    void setQuota(const QuotaContextPtr & quota_) final { stream->setQuota(quota_); }
    void setProcessListElement(QueryStatus * elem) final { stream->setProcessListElement(elem); }
    void setProgressCallback(const ProgressCallback & callback) final { stream->setProgressCallback(callback); }
    void addTotalRowsApprox(size_t value) final { stream->addTotalRowsApprox(value); }

protected:
    void onCancel() override { stream->cancel(false); }

private:
    bool has_aggregate_functions = false;
    bool force_add_aggregating_info = false;
    BlockInputStreamPtr stream;

    RowsBeforeLimitCounterPtr rows_before_limit;

    Chunk totals;
    bool has_totals_port = false;
    bool has_totals = false;

    bool is_generating_finished = false;
    bool is_stream_finished = false;
    bool is_stream_started = false;

    void init();
};

}