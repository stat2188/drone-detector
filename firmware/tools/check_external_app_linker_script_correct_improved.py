#!/usr/bin/env python3

#
# Improved version of check_external_app_linker_script_correct.py
# This version supports dense packing of regions with different sizes
#
# copyleft 2025 zxkmm AKA zix aka sommermorgentraum
#
# This file is part of PortaPack.
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

def parse_memory_regions(ld_file_path):
    regions = []
    
    with open(ld_file_path, 'r') as f:
        content = f.read()
        
    memory_section = re.search(r'MEMORY\s*\{(.*?)\}', content, re.DOTALL)
    if not memory_section:
        print("ERROR: Could not find MEMORY section in the linker script.")
        return []
    
    memory_content = memory_section.group(1)
    
    pattern = r'ram_external_app_(\w+)\s*\(rwx\)\s*:\s*org\s*=\s*(0x[A-Fa-f0-9]+),\s*len\s*=\s*(\d+)k'
    matches = re.finditer(pattern, memory_content)
    
    for match in matches:
        app_name = match.group(1)
        address = int(match.group(2), 16)  # string with hex -> int
        length = int(match.group(3)) * 1024  # kb -> bytes
        
        regions.append({
            'app_name': app_name,
            'address': address,
            'length': length
        })
        
    return sorted(regions, key=lambda x: x['address'])

def validate_memory_regions(regions):
    if not regions:
        print("ERROR: No external app regions found")
        return False
    
    expected_base = 0xADB10000 # the start (not sure why this one)
    issues_found = False
    
    print("\n")
    print(f"checking {len(regions)} external apps address memory regions")
    print("=" * 60)
    
    # Print detailed memory layout
    print("Current Memory Layout:")
    for i, region in enumerate(regions):
        end_addr = region['address'] + region['length']
        print(f"  {i+1:2d}. {region['app_name']:25s} | {hex(region['address']):10s} - {hex(end_addr):10s} | {region['length']//1024:2d}KB")
    
    print("=" * 60)
    
    # Check if first region starts at expected base
    if regions[0]['address'] != expected_base:
        print(f"ERROR: external app first region should start at {hex(expected_base)}, but starts at {hex(regions[0]['address'])}")
        issues_found = True
    
    # Check for overlaps and gaps
    for i, region in enumerate(regions):
        # Check size validity
        expected_sizes = [8 * 1024, 32 * 1024, 48 * 1024]  # 8KB, 32KB, and 48KB
        if region['length'] not in expected_sizes:
            print(f"ERROR: external app region '{region['app_name']}' has incorrect size")
            print(f"     want: 8KB, 32KB, or 48KB, Found: {region['length']//1024}KB")
            issues_found = True
        
        # Check for overlaps with next region
        if i < len(regions) - 1:
            next_region = regions[i + 1]
            current_end = region['address'] + region['length']
            next_start = next_region['address']
            
            if current_end > next_start:
                overlap = current_end - next_start
                print(f"ERROR: external app region '{region['app_name']}' overlaps with '{next_region['app_name']}' by {overlap} bytes")
                print(f"     {region['app_name']} ends at: {hex(current_end)}")
                print(f"     {next_region['app_name']} starts at: {hex(next_start)}")
                issues_found = True
            elif current_end < next_start:
                gap = next_start - current_end
                print(f"WARNING: external app region '{region['app_name']}' has gap of {gap} bytes before '{next_region['app_name']}'")
                print(f"     {region['app_name']} ends at: {hex(current_end)}")
                print(f"     {next_region['app_name']} starts at: {hex(next_start)}")
        
        # Check if address is correct based on previous region (dense packing)
        if i == 0:
            expected_address = expected_base
        else:
            prev_region = regions[i - 1]
            expected_address = prev_region['address'] + prev_region['length']
        
        if region['address'] != expected_address:
            print(f"ERROR: external app region '{region['app_name']}' has incorrect address")
            print(f"     want: {hex(expected_address)}, Found: {hex(region['address'])}")
            issues_found = True
    
    # Check memory limit
    max_address = 0xADEF0000
    if regions:
        last_region = regions[-1]
        last_end = last_region['address'] + last_region['length']
        if last_end > max_address:
            print(f"ERROR: Memory layout exceeds maximum address {hex(max_address)}")
            print(f"     Last region ends at: {hex(last_end)}")
            print(f"     Exceeds by: {last_end - max_address} bytes")
            issues_found = True
    
    print("=" * 60)
    
    if issues_found:
        print("ERROR: Memory address validation failed. Fix the issues before proceeding.")
        return False
    else:
        print("SUCCESS: All external app addresses are correct.")
        return True

def main():
    if len(sys.argv) > 1:
        # arg
        input_path = Path(sys.argv[1])
        if input_path.is_dir():
            ld_file_path = input_path / "external.ld"
            if not ld_file_path.exists():
                print(f"some issue causing that we can't see external app's linker script {input_path}, pass.")
                return
        else:
            ld_file_path = input_path
    else:
        # if no arg, this assume this script is in mayhemrepo/firmware/tools
        ld_file_path = Path("..") / "application" / "external" / "external.ld"
        
    try:
        regions = parse_memory_regions(ld_file_path)
        
        if not regions:
            print("some issue causing that we can't see external app's linker script's address list, pass")

            return
        
        if validate_memory_regions(regions):
            print("external app addr seems correct, pass")
        else:
            print("\nWARNING: It seems you are having incorrect external app addresses.")
            
    except Exception as e:
        print(f"err: {e}")

if __name__ == "__main__":
    main()
