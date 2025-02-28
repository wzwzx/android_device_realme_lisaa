/*
 * Copyright (C) 2022 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android-base/properties.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/system_properties.h>
#include <sys/_system_properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

std::vector<std::string> ro_props_default_source_order = {
    "",
    "odm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
    "vendor_dlkm."
};

/*
 * SetProperty does not allow updating read only properties and as a result
 * does not work for our use case. Write "property_override" to do practically
 * the same thing as "SetProperty" without this restriction.
 */

void property_override(char const prop[], char const value[], bool add = true) {
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const std::string &prop, const std::string &value, bool product = true) {
    string prop_name;

    for (const auto &source : ro_props_default_source_order) {
        if (product)
            prop_name = "ro.product." + source + prop;
        else
            prop_name = "ro." + source + "build." + prop;

        property_override(prop_name.c_str(), value.c_str());
    }
}

void vendor_load_properties() {

  string model;
  string device;
  string name;

/*
 * Only for read-only properties. Properties that can be wrote to more
 * than once should be set in a typical init script (e.g. init.oplus.hw.rc)
 * after the original property has been set.
 */

  auto prj_version = std::stoi(GetProperty("ro.boot.prj_version", "0"));
  
  switch(prj_version){
  	/* OnePlus 10R */
    case 21863:
	device = "OnePlus10R";
	name = "OnePlus10R";
	model = "CPH2411";
	property_override("ro.vendor.usb.name", "OnePlus 10R");
      break;
        /* OnePlus Ace*/
    case 21861:
	device = "OnePlusAce";
	name = "OnePlusAce";
	model = "PGKM10";
	property_override("ro.vendor.usb.name", "OnePlus Ace");}
      break;
    default:
      if (prj_version == "216BE") {
	  device = "RealmeGTNeo3";
	  name = "RealmeGTNeo3";
	  model = "RMX3560";
	  property_override("ro.vendor.usb.name", "Realme GT Neo3");
      } else {
	  device = "OnePlus10R";
	  name = "OnePlus10R";
	  model = "CPH2411";
	  property_override("ro.vendor.usb.name", "OnePlus 8");
	  LOG(ERROR) << "Unexpected project version: " << prj_version;
      }

    set_ro_build_prop("device", device);
    set_ro_build_prop("model", model);
    set_ro_build_prop("name", name);
    set_ro_build_prop("product", model, false);
}
