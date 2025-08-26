#pragma once

#include "ienium/utils/logger/ieniumlogger.hpp"

#define LOGGER ienium::utils::LoggerManager::GetInstance ().GetLogger ()

#define NOT_YET_IMPLEMENTED LOGGER->Log (utils::IENIUM_ERROR, std::string("Error executing ") + __PRETTY_FUNCTION__ + std::string (". This Method is not yet implemented."))