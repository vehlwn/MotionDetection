#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "AvFrameAdapters.hpp"
#include "ScopedAvFormatInput.hpp"
#include "ScopedDecoderContext.hpp"

namespace vehlwn::ffmpeg::detail {
class OutputFile {
public:
    struct Impl;
    OutputFile(std::unique_ptr<Impl>&& pimpl);
    OutputFile(const OutputFile&) = delete;
    OutputFile(OutputFile&& rhs);
    ~OutputFile();

    void encode_write_frame(const OwningAvframe& frame, const int in_stream_index);

private:
    std::unique_ptr<Impl> pimpl;
};

OutputFile open_output_file(
    const char* const url,
    const std::map<int, ScopedDecoderContext>& decoder_contexts,
    const ScopedAvFormatInput::StreamsView in_streams,
    const std::optional<std::string>& video_bitrate,
    const std::optional<std::string>& audio_bitrate);
} // namespace vehlwn::ffmpeg::detail
