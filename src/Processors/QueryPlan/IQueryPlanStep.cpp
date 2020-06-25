#include <Processors/QueryPlan/IQueryPlanStep.h>
#include <Processors/IProcessor.h>
#include <IO/Operators.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int LOGICAL_ERROR;
}

const DataStream & IQueryPlanStep::getOutputStream() const
{
    if (!hasOutputStream())
        throw Exception("QueryPlanStep " + getName() + " does not have output stream.", ErrorCodes::LOGICAL_ERROR);

    return *output_stream;
}

static void doDescribeProcessor(const IProcessor & processor, size_t count, IQueryPlanStep::FormatSettings & settings)
{
    settings.out << String(settings.offset * settings.ident, settings.ident_char) << processor.getName();
    if (count > 1)
        settings.out << " (x" << std::to_string(count) << ")";

    settings.out << '\n';
    ++settings.offset;
}

void IQueryPlanStep::describePipeline(const Processors & processors, FormatSettings & settings)
{
    const IProcessor * prev = nullptr;
    size_t count = 0;

    for (const auto & processor : processors)
    {
        if (prev && prev->getName() != processor->getName())
        {
            doDescribeProcessor(*prev, count, settings);
            count = 0;
        }
        else
            ++count;

        prev = processor.get();
    }

    if (prev)
        doDescribeProcessor(*prev, count, settings);
}

}