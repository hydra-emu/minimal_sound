#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <vector>

#include "hydra/core.hxx"

class HydraCore : public hydra::IBase, public hydra::IFrontendDriven, public hydra::IAudio
{
    HYDRA_CLASS
public:
    bool loadFile(const char* type, const char* path) override
    {
        std::ifstream file(path, std::ios::binary);

        if (file)
        {
            file.seekg(0, std::ios::end);
            auto size = file.tellg();
            file.seekg(0, std::ios::beg);
            samples_.resize(size / sizeof(int16_t));
            file.read((char*)samples_.data(), size);
            return true;
        }

        return false;
    }

    void reset() override
    {
        current_ = samples_.begin();
    }

    virtual hydra::Size getNativeSize() override
    {
        return { 0, 0 };
    }

    void setOutputSize(hydra::Size size) override {}

    void runFrame() override
    {
        if (current_ == samples_.end())
        {
            return;
        }

        if (callback_)
        {
            callback_(&(*current_), 48000 / 60);
            current_ = std::min(current_ + 48000 / 60, samples_.end());
        }
        else
        {
            printf("Audio callback not set?\n");
        }
    }

    uint32_t getSampleRate() override { return 48000; }

    void setAudioCallback(void (*callback)(void*, size_t)) override
    {
        callback_ = callback;
    }

    std::vector<int16_t> samples_;
    std::vector<int16_t>::iterator current_;
    std::function<void(void*, size_t)> callback_;
};