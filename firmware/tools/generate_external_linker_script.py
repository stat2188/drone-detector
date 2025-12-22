#!/usr/bin/env python3

#
# Copyright (C) 2025 PortaPack Mayhem Team
#
# This script generates a conditional external linker script that only
# includes memory regions for enabled external applications.
#

import sys
import os

def generate_conditional_linker_script(external_cmake_path, output_ld_path):
    """
    Generate a conditional linker script that only includes memory regions
    for applications that are not commented out in external.cmake
    """
    
    # Read the external.cmake file
    with open(external_cmake_path, 'r') as f:
        cmake_content = f.read()
    
    # Parse enabled applications from EXTAPPLIST
    # Look for the EXTAPPLIST section and extract application names
    lines = cmake_content.split('\n')
    in_applist = False
    enabled_apps = []
    
    for line in lines:
        line = line.strip()
        if line.startswith('set(EXTAPPLIST'):
            in_applist = True
            continue
        elif line.startswith(')') and in_applist:
            break
        elif in_applist and line and not line.startswith('#'):
            # Extract application name from the line
            # Lines look like: enhanced_drone_analyzer
            app_name = line.strip().rstrip()
            if app_name:
                enabled_apps.append(app_name)
    
    print(f"Found {len(enabled_apps)} enabled applications:")
    for app in enabled_apps:
        print(f"  - {app}")
    
    # Generate the conditional linker script
    ld_content = """/*
    Copyright (C) 2023 Bernd Herzog
    Copyright (C) 2024 Mark Thompson
    Copyright (C) 2025 PortaPack Mayhem Team

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

MEMORY
{
    /*
     * External apps: regions can't overlap so addresses are corrected after build.
     * Picking uncommon address values for search & replace in binaries (no false positives) - 0xADB00000-0xADEF0000 seems to be good.
     * Also need to consider processor memory map - reading 0xADxxxxxx generates a fault which may be better than unexpected behavior.
     * External app address ranges below must match those in python file "external_app_info.py".
     */
"""
    
    # Memory region addresses (same as original)
    base_address = 0xadb10000
    region_size = 32 * 1024  # 32KB per region
    
    # Define memory regions only for enabled applications
    for i, app in enumerate(enabled_apps):
        org = hex(base_address + (i * region_size))
        ld_content += f'    ram_external_app_{app} (rwx) : org = {org}, len = 32k\n'
    
    ld_content += "}\n\nSECTIONS\n{\n"
    
    # Define sections only for enabled applications
    for app in enabled_apps:
        ld_content += f"""    .external_app_{app} : ALIGN(4) SUBALIGN(4)
    {{
        KEEP(*(.external_app.app_{app}.application_information));
        *(*ui*external_app*{app}*);
    }} > ram_external_app_{app}

"""
    
    ld_content += "}\n"
    
    # Write the generated linker script
    with open(output_ld_path, 'w') as f:
        f.write(ld_content)
    
    print(f"Generated conditional linker script: {output_ld_path}")
    print(f"Memory regions defined for {len(enabled_apps)} applications")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: generate_external_linker_script.py <external.cmake path> <output .ld path>")
        sys.exit(1)
    
    external_cmake_path = sys.argv[1]
    output_ld_path = sys.argv[2]
    
    if not os.path.exists(external_cmake_path):
        print(f"Error: {external_cmake_path} not found")
        sys.exit(1)
    
    generate_conditional_linker_script(external_cmake_path, output_ld_path)
