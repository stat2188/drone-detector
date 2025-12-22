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
    print("Starting external app address fix...")
    
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
    
    if not regions:
        print("ERROR: No external app regions found to fix")
        return []
    
    # Sort by current address
    regions.sort(key=lambda x: x['address'])
    
    # Validate that all regions have valid sizes
    valid_sizes = [8 * 1024, 32 * 1024, 48 * 1024]
    for region in regions:
        if region['length'] not in valid_sizes:
            print(f"ERROR: Region '{region['app_name']}' has invalid size: {region['length']//1024}KB")
            print("Only 8KB, 32KB, and 48KB sizes are supported")
            return []
    
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
    
    # Validate that we don't exceed memory limits
    max_address = 0xADEF0000  # Maximum allowed address
    if current_address > max_address:
        print(f"ERROR: Memory layout would exceed maximum address {hex(max_address)}")
        print(f"Total required memory: {hex(current_address - 0xADB10000)}")
        return []
    
    # Create backup
    backup_path = ld_file_path + ".backup"
    with open(backup_path, 'w') as f:
        f.write(content)
    print(f"Created backup: {backup_path}")
    
    # Update the content
    modified_content = content
    changes_made = 0
    
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
            if pattern in modified_content:
                modified_content = modified_content.replace(pattern, f"org = {new_addr_hex}, len = {region['length']//1024}k")
                changes_made += 1
                break
    
    # Write back to file
    with open(ld_file_path, 'w') as f:
        f.write(modified_content)
    
    # Print summary
    print(f"SUCCESS: Fixed {changes_made} external app addresses for dense packing:")
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
