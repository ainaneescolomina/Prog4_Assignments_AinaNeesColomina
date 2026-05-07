#include "ServiceLocator.h"

namespace dae
{
    std::unique_ptr<ISoundSystem> servicelocator::_ss_instance{ std::make_unique<NullSoundSystem>() };
}