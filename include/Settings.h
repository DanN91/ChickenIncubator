/*
 * Settings.h
 *
 * This file contains the definition of the GetSettingAddress function, which maps
 * different settings to their respective memory addresses. It is used to retrieve
 * the address of a specific setting in the settings storage.
 */

#pragma once

#include <SettingsMasks.h>

uint16_t GetSettingAddress(Settings setting)
{
  switch (setting)
  {
    case Settings::IncubationTemperature: // 8 bytes
      return 0;

    case Settings::IncubationHumidity: // 2 bytes
      return 8;

    case Settings::LockdownTemperature: // 8 bytes
      return 10;

    case Settings::LockdownHumidity: // 2 bytes
      return 18;

    case Settings::CycleState: // 1 byte
      return 20;

    case Settings::ChangeStateDate: // 3 bytes
      return 21;

    case Settings::ChangeStateTime: // 4 bytes
      return 24;

    default:
      return -1; // max uint16_t
  }
}