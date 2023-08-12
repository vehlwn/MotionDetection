#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "../ApplicationSettings.hpp"
#include "AvFrameAdapters.hpp"
#include "ScopedAvFormatInput.hpp"
#include "ScopedDecoderContext.hpp"

namespace vehlwn::ffmpeg::detail {
class OutputFile {
public:
    struct Impl;
    explicit OutputFile(std::unique_ptr<Impl>&& pimpl);
    OutputFile(const OutputFile&) = delete;
    OutputFile(OutputFile&& rhs) noexcept;
    ~OutputFile();
    OutputFile& operator=(const OutputFile&) = delete;
    OutputFile& operator=(OutputFile&&) noexcept;

    void encode_write_frame(const OwningAvframe& frame, int in_stream_index);

private:
    std::unique_ptr<Impl> pimpl;
};

OutputFile open_output_file(
    std::shared_ptr<const ApplicationSettings>&& settings,
    const char* url,
    const std::map<int, ScopedDecoderContext>& decoder_contexts,
    ScopedAvFormatInput::StreamsView in_streams);
} // namespace vehlwn::ffmpeg::detail
