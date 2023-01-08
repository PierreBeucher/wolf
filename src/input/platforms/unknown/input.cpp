#include <helpers/logger.hpp>
#include <input/input.hpp>

namespace input {

immer::array<immer::box<dp::handler_registration>> setup_handlers(std::size_t session_id,
                                                                  std::shared_ptr<dp::event_bus> event_bus) {
  logs::log(logs::error, "Unable to setup input handlers for the current platform.");
  return {};
}
} // namespace input