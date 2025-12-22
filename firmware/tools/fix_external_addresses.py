#!/usr/bin/env python3

#
# Script to automatically fix external app addresses for dense packing
#
# Copyright (C) 2025 PortaPack Team
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import re
import sys
from pathlib import Path

def fix_external_addresses(ld_file_path):
    """
    Automatically fix external app addresses for dense packing.
    """
    with open(ld_file_path, 'r') as f:
        content = f.read()
    
    # Parse current regions
    regions = []
    pattern = r'ram_external_app_(\w+)\s*\(rwx\)\s*:\s*org\s*=\s*(0x[A-Fa-f0-9]+),\s*len\s*=\s*(\d+)k'
    matches = re.finditer(pattern, content)
    
    for match in matches:
        app_name = match.group(1)
        address = int(match.group(2), 16)
        length = int(match.group(3)) * 1024
        regions.append({
            'app_name': app_name,
            'address': address,
            'length': length
        })
    
    # Sort by current address
    regions.sort(key=lambda x: x['address'])
    
    # Calculate new addresses for dense packing
    current_address = 0xADB10000
    new_regions = []
    
    for region in regions:
        new_region = {
            'app_name': region['app_name'],
            'old_address': region['address'],
            'new_address': current_address,
            'length': region['length']
        }
        new_regions.append(new_region)
        current_address += region['length']
    
    # Update the content
    for region in new_regions:
        old_addr_hex = hex(region['old_address'])
        new_addr_hex = hex(region['new_address'])
        
        # Replace the address in the content with different hex formats
        patterns_to_replace = [
            f"org = {old_addr_hex}, len = {region['length']//1024}k",
            f"org = {old_addr_hex.upper()}, len = {region['length']//1024}k",
            f"org = 0x{region['old_address']:08X}, len = {region['length']//1024}k",
            f"org = 0x{region['old_address']:08x}, len = {region['length']//1024}k",
        ]
        
        for pattern in patterns_to_replace:
            if pattern in content:
                content = content.replace(pattern, f"org = {new_addr_hex}, len = {region['length']//1024}k")
                break
    
    # Write back to file
    with open(ld_file_path, 'w') as f:
        f.write(content)
    
    # Print summary
    print("Fixed external app addresses for dense packing:")
    for region in new_regions:
        print(f"  {region['app_name']}: {hex(region['old_address'])} -> {hex(region['new_address'])}")
    
    return new_regions

def main():
    if len(sys.argv) < 2:
        print("Usage: fix_external_addresses.py <external.ld>")
        sys.exit(1)
    
    ld_file_path = sys.argv[1]
    if not Path(ld_file_path).exists():
        print(f"ERROR: File not found: {ld_file_path}")
        sys.exit(1)
    
    print(f"Fixing addresses in: {ld_file_path}")
    new_regions = fix_external_addresses(ld_file_path)
    print(f"Fixed {len(new_regions)} regions")

if __name__ == "__main__":
    main()
