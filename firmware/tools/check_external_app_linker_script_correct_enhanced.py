#!/usr/bin/env python3

#
# Enhanced version of check_external_app_linker_script_correct.py
# This version includes size monitoring, early failure detection, and better validation
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
import os
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
    total_memory = 0
    
    print("\n")
    print(f"checking {len(regions)} external apps address memory regions")
    print("=" * 80)
    
    # Print detailed memory layout with calculations
    print("Current Memory Layout:")
    print(f"{'#':>2} | {'App Name':<25} | {'Start Address':<12} | {'End Address':<12} | {'Size':<6} | {'Cumulative':<8}")
    print("-" * 80)
    
    cumulative_size = 0
    for i, region in enumerate(regions):
        end_addr = region['address'] + region['length']
        cumulative_size += region['length']
        total_memory += region['length']
        size_kb = region['length'] // 1024
        cumulative_kb = cumulative_size // 1024
        
        print(f"{i+1:2d} | {region['app_name']:25s} | {hex(region['address']):10s} | {hex(end_addr):10s} | {size_kb:4d}KB | {cumulative_kb:6d}KB")
    
    print("=" * 80)
    print(f"Total Memory Used: {total_memory} bytes ({total_memory//1024} KB)")
    print(f"SPI Flash Limit: 1048576 bytes (1024 KB)")
    print(f"Memory Usage: {round(100 * total_memory / 1048576, 1)}%")
    print("=" * 80)
    
    # Check if we're approaching or exceeding the limit
    if total_memory > 1048576:
        print(f"ERROR: Total memory usage ({total_memory} bytes) exceeds SPI flash limit (1048576 bytes)")
        issues_found = True
    elif total_memory > 943718:  # 90% of 1MB
        print(f"WARNING: Memory usage is at {round(100 * total_memory / 1048576, 1)}% of SPI flash limit")
    elif total_memory > 838860:  # 80% of 1MB
        print(f"WARNING: Memory usage is at {round(100 * total_memory / 1048576, 1)}% of SPI flash limit")
    
    # Check if first region starts at expected base
    if regions[0]['address'] != expected_base:
        print(f"ERROR: external app first region should start at {hex(expected_base)}, but starts at {hex(regions[0]['address'])}")
        issues_found = True
    
    # Check for overlaps and gaps
    for i, region in enumerate(regions):
        # Check size validity
        expected_sizes = [8 * 1024, 16 * 1024, 32 * 1024, 48 * 1024]  # 8KB, 16KB, 32KB, and 48KB
        if region['length'] not in expected_sizes:
            print(f"ERROR: external app region '{region['app_name']}' has incorrect size")
            print(f"     want: 8KB, 16KB, 32KB, or 48KB, Found: {region['length']//1024}KB")
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
    
    print("=" * 80)
    
    if issues_found:
        print("ERROR: Memory address validation failed. Fix the issues before proceeding.")
        return False
    else:
        print("SUCCESS: All external app addresses are correct.")
        return True

def check_application_sizes(regions):
    """
    Check if applications are using appropriate memory sizes.
    This is a basic check - in a real implementation, you'd want to
    measure actual binary sizes and compare against allocated regions.
    """
    print("\nMemory Size Analysis:")
    print("=" * 50)
    
    size_distribution = {8: 0, 16: 0, 32: 0, 48: 0}
    for region in regions:
        size_kb = region['length'] // 1024
        size_distribution[size_kb] += 1
    
    for size, count in size_distribution.items():
        if count > 0:
            print(f"  {size:2d}KB regions: {count} applications")
    
    # Check for potential optimizations
    large_regions = [r for r in regions if r['length'] == 32*1024 or r['length'] == 48*1024]
    if len(large_regions) > 10:
        print(f"  WARNING: {len(large_regions)} applications using 32KB+ regions")
        print("           Consider optimizing application sizes or removing unused features")
    
    print("=" * 50)

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
        
        # Run enhanced validation
        validation_passed = validate_memory_regions(regions)
        
        # Additional size analysis
        check_application_sizes(regions)
        
        if validation_passed:
            print("external app addr seems correct, pass")
            return 0
        else:
            print("\nWARNING: It seems you are having incorrect external app addresses.")
            return 1
            
    except Exception as e:
        print(f"err: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
